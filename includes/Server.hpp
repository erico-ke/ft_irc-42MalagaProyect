/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 12:22:34 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/14 16:06:14 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "./Client.hpp"
# include "./Channel.hpp"
# include "./CommandHandler.hpp"
# include "./CodeUtils.hpp"
# include <vector>
# include <map>
# include <string>
# include <poll.h>
# include <stdexcept>
# include <iostream>
# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netinet/in.h>

extern bool g_running;

/* ===== MAIN CLASS ===== */
class Server
{
private:
	int								_port;
	std::string						_password;
	int								_serverFd;
	std::vector<struct pollfd>		_pollfds;
	std::map<int, Client*>			_clients;
	std::map<std::string, Channel*>	_channels;

	void	_initSocket();
	void	_acceptClient();
	void	_handleClient(int fd);
public:
	Server() {throw invalidConstructorCall();}
	Server(int port, const std::string &password);
	Server(const Server &other) {(void)other; throw invalidConstructorCall();}
	Server &operator=(const Server &other) {(void)other; throw invalidConstructorCall();}
	~Server(void);
	
	void	run();

	void		sendToClient(int fd, const std::string &msg);
	void		removeClient(int fd);
	Client		*getClientByNick(const std::string &nick);
	Channel		*getChannel(const std::string &name);
	Channel		*getOrCreateChannel(const std::string &name);
	bool		isNickInUse(const std::string &nick) const;
	std::string	getPassword() const;
	std::string	getServerName() const;
};

#endif