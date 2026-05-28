/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 10:51:46 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/28 09:21:44 by fracurul         ###   ########.fr       */
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

class	Server;

class	CommandHandler
{
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
		static void						_tryFinishAuth(Client &client, Server &server);
};

#endif
