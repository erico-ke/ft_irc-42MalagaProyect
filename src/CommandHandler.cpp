/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 14:07:06 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/25 14:30:40 by erico-ke         ###   ########.fr       */
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

void	CommandHandler::handleJoin(Client &client, const std::string &params, Server &server)
{
	if (!client.isAuth()) return ;

	std::vector<std::string> args = splitParams(params);
	if (args.empty())
	{
		server.sendToClient(client.getFd(), ":irscerv 461 JOIN :Not enough parameters\r\n");
		return ;
	}
	
	std::string	chanName = args[0];
	std::string	key = (args.size() > 1) ? args[1] : "";
	
	if (chanName[0] != '#')
	{
		server.sendToClient(client.getFd(), ":ircserv 403 " + chanName + " :No such channel\r\n");
		return ;
	}
	
	Channel	*chan = server.getOrCreateChannel(chanName);

	if (chan->isInviteOnly() && !chan->isInvited(&client))
	{
		server.sendToClient(client.getFd(), ":ircserv 473 " + client.getNickname() + " " + chanName + " :Cannot join (invite only)\r\n");
		return ;
	}

	if (!chan->getKey().empty() && chan->getKey() != key)
	{
		server.sendToClient(client.getFd(), ":ircserv 475 " + client.getNickname() + " " + chanName + " :Bad channel key\r\n");
		return ;
	}
	
	if (chan->getUserLimit() != -1 && (int)chan->getMemberCount() >= chan->getUserLimit())
	{
		server.sendToClient(client.getFd(), ":ircserv 471 " + client.getNickname() + " " + chanName + " :Channel is full\r\n");
		return ;
	}
	
	bool	firstMember = chan->isEmpty();
	chan->addMember(&client);
	if (firstMember)
		chan->addOperator(&client);

	std::string	joinmsg = client.getPrefix() + " JOIN " + chanName + "\r\n";
	chan->broadcast(joinmsg);

	if (chan->getTopic().empty())
		server.sendToClient(client.getFd(), ":ircserv 331 " + client.getNickname() + " " + chanName + " :No topic is set\r\n");
	else
		server.sendToClient(client.getFd(), ":ircserv 332 " + client.getNickname() + " " + chanName + " :" + chan->getTopic() + "\r\n");
}

