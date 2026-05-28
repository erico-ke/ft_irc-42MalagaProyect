/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JoinCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 10:01:53 by fracurul          #+#    #+#             */
/*   Updated: 2026/05/28 10:12:04 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands/JoinCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/commands/CommandHandler.hpp"
#include <sstream>
#include <algorithm>

void	JoinCommand::execute(Client& client, const std::string& params, Server& server)
{
	if (!client.isAuth()) { return ; }

	std::vector<std::string>	args = CommandHandler::splitParams(params);
	if (args.empty())
	{
		server.sendToClient(client.getFd(), ":irscerv 461 JOIN :Not enough parameters\r\n");
		return ;
	}

	std::string	chanName = args[0];
	std::string	key = (args.size() > 1) ? args[1] : "";
	if (!chanName[1])
	{
		server.sendToClient(client.getFd(), ":ircserv 461 JOIN :Not enough parameters (# has to be followed by something(not a space))\r\n");
		return ;
	}
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
	if (firstMember) { chan->addOperator(&client); }

	std::string	joinmsg = client.getPrefix() + " JOIN " + chanName + "\r\n";
	chan->broadcast(joinmsg, server);
	if (chan->getTopic().empty())
		server.sendToClient(client.getFd(), ":ircserv 331 " + client.getNickname() + " " + chanName + " :No topic is set\r\n");
	else
		server.sendToClient(client.getFd(), ":ircserv 332 " + client.getNickname() + " " + chanName + " :" + chan->getTopic() + "\r\n");
}
