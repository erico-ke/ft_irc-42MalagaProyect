/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 10:51:46 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/26 11:34:18 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include "../Client.hpp"
# include "../Channel.hpp"
# include "../CodeUtils.hpp"
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
	static void	handleQuit(Client &client, const std::string &params, Server &server);

	static void	_tryFinishAuth(Client &client, Server &server);

public:
	CommandHandler(void) {throw invalidConstructorCall();}
	CommandHandler(const CommandHandler &other) {(void)other; throw invalidConstructorCall();}
	CommandHandler &operator=(const CommandHandler &other) {(void)other; throw invalidConstructorCall();}
	~CommandHandler(void);

	// Handler orchestor for Command classes
	static void	handle(Client &client, const std::string &line, Server &server);

	// Utility methods for parsing (used by Command classes)
	static std::string				toUpper(const std::string &s);
	static std::vector<std::string>	splitParams(const std::string &params);
	static std::string				getTrailing(const std::string &params);
};

#endif
