/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:46:01 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/13 15:12:04 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include "Client.hpp"
# include "Channel.hpp"
# include "CodeUtils.hpp"
# include <string>
# include <vector>
# include <iostream>
# include <sstream>
# include <algorithm>
# include <cctype>

class Server;

/* ===== MAIN CLASS ===== */
class CommandHandler
{
private:
	static void	handlePass(Client &client, const std::string &params, Server &server);
	static void	handleNick(Client &client, const std::string &params, Server &server);
	static void	handleUser(Client &client, const std::string &params, Server &server);
	static void	handleJoin(Client &client, const std::string &params, Server &server);
	static void	handlePrivmsg(Client &client, const std::string &params, Server &server);
	static void	handleKick(Client &client, const std::string &params, Server &server);
	static void	handleInvite(Client &client, const std::string &params, Server &server);
	static void	handleTopic(Client &client, const std::string &params, Server &server);
	static void	handleMode(Client &client, const std::string &params, Server &server);
	static void	handleQuit(Client &client, const std::string &params, Server &server);

	static void	_tryFinishAuth(Client &client, Server &server);

	static std::string				toUpper(const std::string &s);
	static std::vector<std::string>	splitParams(const std::string &params);
	static std::string				getTrailing(const std::string &params);
public:
	CommandHandler(void) {throw invalidConstructorCall();}
	CommandHandler(const CommandHandler &other) {(void)other; throw invalidConstructorCall();}
	CommandHandler &operator=(const CommandHandler &other) {(void)other; throw invalidConstructorCall();}
	~CommandHandler(void);
	
	static void	handle(Client &client, const std::string &line, Server &server);
};

#endif