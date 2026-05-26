/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 10:51:36 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/26 15:16:14 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands/CommandHandler.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/commands/PassCommand.hpp"
#include "../../includes/commands/NickCommand.hpp"
#include "../../includes/commands/UserCommand.hpp"
#include "../../includes/commands/JoinCommand.hpp"
#include "../../includes/commands/PrivmsgCommand.hpp"
#include "../../includes/commands/KickCommand.hpp"
#include "../../includes/commands/InviteCommand.hpp"
#include "../../includes/commands/ModeCommand.hpp"
#include "../../includes/commands/TopicCommand.hpp"
#include "../../includes/commands/QuitCommand.hpp"


void::CommandHandler::handle(Client &client, const std::string &line, Server &server)
{
	if (line.empty()) return ;

	std::istringstream iss(line);
	std::string	command;
	iss >> command;
	command = toUpper(command);

	std::string	params;
	std::getline(iss, params);
	if (!params.empty() && params[0] == ' ')
		params = params.substr(1);

	if (command == "PASS") { PassCommand cmd; cmd.execute(client, params, server); }
	else if (command == "NICK") { NickCommand cmd; cmd.execute(client, params, server); }
	else if (command == "USER") { UserCommand cmd; cmd.execute(client, params, server); }
	else if (command == "QUIT") { QuitCommand cmd; cmd.execute(client, params, server); }
	else if (command == "JOIN") { JoinCommand cmd; cmd.execute(client, params, server); }
	else if (command == "PRIVMSG") { PrivmsgCommand cmd; cmd.execute(client, params, server); }
	else if (command == "KICK") { KickCommand cmd; cmd.execute(client, params, server); }
	else if (command == "INVITE") { InviteCommand cmd; cmd.execute(client, params, server); }
	else if (command == "TOPIC") { TopicCommand cmd; cmd.execute(client, params, server); }
	else if (command == "MODE") { ModeCommand cmd; cmd.execute(client, params, server); }
	else { server.sendToClient(client.getFd(), ":server 421 " + command + " :Unknown command\r\n"); }
}

std::string	CommandHandler::toUpper(const std::string &s)
{
	std::string	result = s;
	for (size_t i = 0; i < result.size(); i++)
		result[i] = toupper(result[i]);
	return result;
}

std::string	CommandHandler::getTrailing(const std::string &params)
{
	size_t pos = params.find(':');
	if (pos == std::string::npos) return "";
	return  params.substr(pos + 1);
}

std::vector<std::string>	CommandHandler::splitParams(const std::string &params)
{
	std::vector<std::string>	result;
	std::istringstream			iss(params);
	std::string					token;
	while (iss >> token)
	{
		if (token[0] == ':') break;
		result.push_back(token);
	}
	return result;
}

void CommandHandler::_tryFinishAuth(Client &client, Server &server)
{
	if (!client.passGiven() || !client.nickGiven() || !client.userGiven())	return;

	client.setAuth(true);

	std::string nick = client.getNickname();
	server.sendToClient(client.getFd(), ":ircserv 001 " + nick + " :Welcome to the IRC Network, " + nick + "!\r\n");
	server.sendToClient(client.getFd(), ":ircserv 002 " + nick + " :Your host is ircserv, running version 1.0\r\n");
	server.sendToClient(client.getFd(), ":ircserv 003 " + nick + " :This server was created today\r\n");
	server.sendToClient(client.getFd(), ":ircserv 004 " + nick + " ircserv 1.0 o itkol\r\n");
}
