/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:25:10 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/28 11:37:23 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(int port, const std::string &password) : _port(port), _password(password), _serverFd(-1)
{
	DEBUG_LOG("Server constructor called");
	_initSocket();
}

Server::~Server()
{
	DEBUG_LOG("Server destructor called");
	if (_serverFd >= 0) { close(_serverFd); }
	for (std::map<std::string, Channel *>::iterator	it = _channels.begin(); it != _channels.end(); ++it)
		delete (it->second);
	_channels.clear();

	for (std::map<int, Client *>::iterator	it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first >= 0) { close(it->first); }
		delete (it->second);
	}
	_clients.clear();
}

void	Server::_initSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0) { throw std::runtime_error("socket() failed"); }

	int	opt = 1;
	setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	fcntl(_serverFd, F_SETFL, O_NONBLOCK);
	struct sockaddr_in	addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);
	if (bind(_serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { throw std::runtime_error("bind() failed"); }
	if (listen(_serverFd, 10) < 0) { throw std::runtime_error("listen() failed"); }

	struct pollfd	pfd;
	pfd.fd = _serverFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollfds.push_back(pfd);
	std::cout << "Server initialized on port " << _port << std::endl;
}

void	Server::run()
{
	std::cout << "Server running. Waiting for connections..." << std::endl;
	while (g_running)
	{
		int	ret = poll(_pollfds.data(), _pollfds.size(), -1);
		if (ret < 0)
		{
			if (!g_running) { break ; }
			throw std::runtime_error("poll() failed");
		}
		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			int		fd = _pollfds[i].fd;
			short	revents = _pollfds[i].revents;
			if (fd == _serverFd)
			{
				if (revents & POLLIN) { _acceptClient(); }
				continue ;
			}
			if (revents & POLLIN) { _handleClient(fd); }
			if (_clients.find(fd) == _clients.end()) { continue ; }
			if (revents & POLLOUT) { _flushClientOutput(fd); }
		}
	}
	std::cout << "Server shutting down." << std::endl;
}

void	Server::_acceptClient()
{
	struct sockaddr_in	clientAddr;
	socklen_t			len = sizeof(clientAddr);

	int	clientFd = accept(_serverFd, (struct sockaddr*)&clientAddr, &len);
	if (clientFd < 0) { return ; }

	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	struct pollfd	pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollfds.push_back(pfd);
	_clients[clientFd] = new Client(clientFd);
	std::cout << "New client connected: fd = " << clientFd << std::endl;
}

void	Server::removeClient(int fd)
{
	std::map<int, Client *>::iterator	clientIt = _clients.find(fd);
	if (clientIt == _clients.end()) { return ; }

	for (std::map<std::string, Channel *>::iterator	it = _channels.begin(); it != _channels.end(); ++it)
		it->second->removeMember(clientIt->second);
	for (std::vector<struct pollfd>::iterator	it = _pollfds.begin(); it != _pollfds.end(); ++it)
		if (it->fd == fd) { _pollfds.erase(it); break ; }
	close(fd);
	delete (clientIt->second);
	_clients.erase(clientIt);
	std::cout << "Client disconnected: fd = " << fd << std::endl;
}

void	Server::sendToClient(int fd, const std::string &msg)
{
	std::map<int, Client *>::iterator	clientIt = _clients.find(fd);
	if (clientIt == _clients.end()) { return ; }

	clientIt->second->appendToOutBuffer(msg);
	_setPollEvents(fd, POLLIN | POLLOUT);
}

void	Server::_handleClient(int fd)
{
	std::map<int, Client *>::iterator	clientIt = _clients.find(fd);
	if (clientIt == _clients.end()) { return ; }

	Client	*client = clientIt->second;
	char	buf[512];
	memset(buf, 0, sizeof(buf));
	int	bytes = recv(fd, buf, sizeof(buf) - 1, 0);
	if (bytes <= 0) { removeClient(fd); return ; }

	client->appendToBuffer(std::string(buf, bytes));
	std::string	&buffer = client->getBufferRef();
	size_t		pos;
	while ((pos = buffer.find("\n")) != std::string::npos)
	{
		if (buffer.find("\r") != std::string::npos) { pos = buffer.find("\r"); }

		std::string	line = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);
		if (!line.empty()) { CommandHandler::handle(*client, line, *this); }
		if (_clients.find(fd) == _clients.end()) { return ; }
	}

}

void	Server::_flushClientOutput(int fd)
{
	std::map<int, Client *>::iterator	clientIt = _clients.find(fd);
	if (clientIt == _clients.end()) { return ; }

	Client	*client = clientIt->second;
	std::string &outBuffer = client->getOutBufferRef();
	if (outBuffer.empty()) { _setPollEvents(fd, POLLIN); return ; }

	ssize_t	sent = send(fd, outBuffer.c_str(), outBuffer.size(), 0);
	if (sent <= 0) { removeClient(fd); return ; }

	outBuffer.erase(0, sent);
	if (outBuffer.empty()) { _setPollEvents(fd, POLLIN); }
}

void	Server::_setPollEvents(int fd, short events)
{
	for (size_t i = 0; i < _pollfds.size(); i++)
		if (_pollfds[i].fd == fd) { _pollfds[i].events = events; break ; }
}

// * Getters * //
Client	*Server::getClientByNick(const std::string &nick)
{
	for (std::map<int, Client *>:: iterator	it = _clients.begin(); it != _clients.end(); ++it)
		if (it->second->getNickname() == nick) { return (it->second); }
	return(NULL);
}
Channel	*Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator	it = _channels.find(name);
	if (it != _channels.end()) { return (it->second); }
	return(NULL);
}
Channel	*Server::getOrCreateChannel(const std::string &name)
{
	Channel	*channel = getChannel(name);
	if (channel == NULL) { _channels[name] = new Channel(name); }
	return (_channels[name]);
}
std::vector<Channel *>	Server::getChannelsForClient(Client *client) const
{
	std::vector<Channel *>	channels;
	for (std::map<std::string, Channel *>::const_iterator	it = _channels.begin(); it != _channels.end(); ++it)
		if (it->second->isMember(client)) { channels.push_back(it->second); }
	return (channels);
}
bool	Server::isNickInUse(const std::string &nick) const
{
	for (std::map<int, Client *>::const_iterator	it = _clients.begin(); it != _clients.end(); ++it)
		if (it->second->getNickname() == nick) { return (true); }
	return (false);
}
std::string	Server::getPassword() const { return (_password); }
std::string	Server::getServerName() const { return ("ft_irc"); }