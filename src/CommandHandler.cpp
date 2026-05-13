/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 14:07:06 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/13 15:15:19 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CommandHandler.hpp"
#include "../includes/Server.hpp"

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

	if (command == "PASS")		handlePass(client, params, server);
	else if (command == "NICK")	handleNick(client, params, server);
	else if	(command == "USER")	handleUser(client, params, server);
	else if	(command == "QUIT")	handleQuit(client, params, server);
	else if	(command == "JOIN")	handleJoin(client, params, server);
	else if	(command == "PRIVMSG")	handlePrivmsg(client, params, server);
	else if	(command == "KICk")	handleKick(client, params, server);
	else if	(command == "INVITE")	handleInvite(client, params, server);
	else if	(command == "TOPIC")	handleTopic(client, params, server);
	else if	(command == "MODE")	handleMode(client, params, server);
	else	server.sendToClient(client.getFd(), ":server 421 " + command + ":Unknown command\r\n");
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