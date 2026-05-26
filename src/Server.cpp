/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:25:10 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/26 13:19:26 by erico-ke         ###   ########.fr       */
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
	if (_serverFd >= 0)
		close(_serverFd);

	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
	
	for (size_t i = 0; i < _clients.size(); i++)
		delete(_clients[i]);
}

void	Server::_initSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		throw std::runtime_error("socket() failed");

	int	opt = 1;
	setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	fcntl(_serverFd, F_SETFL, O_NONBLOCK);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);

	if (bind(_serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind() failed");

	if (listen(_serverFd, 10) < 0)
		throw std::runtime_error("listen() failed");

	struct pollfd pfd;
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
		int ret = poll(_pollfds.data(), _pollfds.size(), -1);

		if (ret < 0)
		{
			if (!g_running) break;
			throw std::runtime_error("poll() failed");
		}

		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (!(_pollfds[i].revents & POLLIN))
				continue ;
			if (_pollfds[i].fd == _serverFd)
				_acceptClient();
			else
				_handleClient(_pollfds[i].fd);
		}
	}
	std::cout << "Server shutting down." << std::endl;
}

void	Server::_acceptClient()
{
	struct sockaddr_in	clientAddr;
	socklen_t	len = sizeof(clientAddr);

	int clientFd = accept(_serverFd, (struct sockaddr*)&clientAddr, &len);
	if (clientFd < 0)
		return ;

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
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		it->second->removeMember(_clients[fd]);

	for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_pollfds.erase(it);
			break ;
		}
	}

	close(fd);
	delete _clients[fd];
	_clients.erase(fd);

	std::cout << "Client disconnected: fd = " << fd << std::endl;
}

void	Server::sendToClient(int fd, const std::string &msg)
{
	if (send(fd, msg.c_str(), msg.size(), 0) < 0)
		removeClient(fd);
}

void	Server::_handleClient(int fd)
{
	char	buf[512];
	memset(buf, 0, sizeof(buf));

	int	bytes = recv(fd, buf, sizeof(buf) - 1, 0);

	if (bytes <= 0)
	{
		removeClient(fd);
		return ;
	}

	_clients[fd]->appendToBuffer(std::string(buf, bytes));

	std::string	&buffer = _clients[fd]->getBufferRef();
	size_t	pos;
	while ((pos = buffer.find("\r\n")) != std::string::npos)
	{
		std::string	line = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);
		if (!line.empty())
			CommandHandler::handle(*_clients[fd], line, *this);
	}
}

// * Getters * //

Client	*Server::getClientByNick(const std::string &nick)
{
	for (std::map<int, Client*>:: iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nick)
			return(it->second);
	}
	return(NULL);
}

Channel	*Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return(it->second);
	return(NULL);
}

Channel	*Server::getOrCreateChannel(const std::string &name)
{
	Channel	*channel = getChannel(name);
	if (channel == NULL)
		_channels[name] = new Channel(name);
	return(_channels[name]);
}

bool	Server::isNickInUse(const std::string &nick) const
{
	for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nick)
			return(true);
	}
	return (false);
}

std::string	Server::getPassword() const { return(_password); }

std::string	Server::getServerName() const { return("ft_irc"); }