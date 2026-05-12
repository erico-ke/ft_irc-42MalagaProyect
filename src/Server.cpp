/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:25:10 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/12 15:01:10 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"

const char	*Server::invalidConstructorCall::what() const throw()
{
	return ("Invalid constructor call");
}

Server::Server(int port, const std::string &password) : _port(port), _password(password), _serverFd(-1)
{
	_initSocket();
}

Server::~Server()
{
	if (_serverFd >= 0)
		close(_serverFd);
	
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
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

extern bool g_running;

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

