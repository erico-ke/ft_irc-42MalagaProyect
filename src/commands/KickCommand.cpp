/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KickCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 10:01:49 by fracurul          #+#    #+#             */
/*   Updated: 2026/05/28 11:24:10 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands/KickCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/commands/CommandHandler.hpp"
#include <sstream>
#include <algorithm>

void	KickCommand::execute(Client& client, const std::string& params, Server& server)
{
	if (!client.isAuth()) { return ; }

	std::vector<std::string>	args = CommandHandler::splitParams(params);
	std::string					reason = CommandHandler::getTrailing(params);
	if (reason.empty()) { reason = "Kicked"; }
	if (args.size() < 2)
	{
		server.sendToClient(client.getFd(), ":ircserv 461 KICK :Not enough parameters\r\n");
		return ;
	}

	std::string	chanName = args[0];
	std::string	targetNick = args[1];
	Channel		*chan = server.getChannel(chanName);
	if (!chan)
	{
		server.sendToClient(client.getFd(), ":ircserv 403 " + chanName + " :No such channel\r\n");
		return ;
	}
	if (!chan->isOperator(&client))
	{
		server.sendToClient(client.getFd(),  ":ircserv 482 " + chanName + " :You're not channel operator\r\n");
		return ;
	}

	Client	*target = server.getClientByNick(targetNick);
	if (!target || !chan->isMember(target))
	{
		server.sendToClient(client.getFd(),  ":ircserv 441 " + targetNick + " " + chanName + " :They aren't on that channel\r\n");
		return ;
	}

	std::string	kickMsg = client.getPrefix() + " KICK " + chanName + " " + targetNick + " :" + reason + "\r\n";
	chan->broadcast(kickMsg, server);
	chan->removeMember(target);
}
