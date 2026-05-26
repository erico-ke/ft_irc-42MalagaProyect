/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TopicCommand.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 11:03:24 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/26 11:26:12 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands/TopicCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/commands/CommandHandler.hpp"

void	TopicCommand::execute(Client &client, const std::string &params, Server &server)
{
	if (!client.isAuth()) return ;

	std::vector<std::string> args = CommandHandler::splitParams(params);
	if (args.empty())
	{
		server.sendToClient(client.getFd(), ":ircserv 461 TOPIC :Not enough parameters\r\n");
		return ;
	}
	
	std::string	chanName = args[0];
	Channel	*chan = server.getChannel(chanName);
	if (!chan)
	{
		server.sendToClient(client.getFd(), ":ircserv 403 " + chanName + " :No such channel\r\n");
		return ;
	}
	if (!chan->isMember(&client))
	{
		server.sendToClient(client.getFd(), ":ircserv 442 " + chanName + " :You're not on that channel\r\n");
		return ;
	}

	size_t	colonPos = params.find(':');
	if (colonPos == std::string::npos)
	{
		if (chan->getTopic().empty())
			server.sendToClient(client.getFd(), ":ircserv 331 " + client.getNickname() + " " + chanName + " :No topic is set\r\n");
		else
			server.sendToClient(client.getFd(), ":ircserv 332 " + client.getNickname() + " " + chanName + " :" + chan->getTopic() + "\r\n");
		return ;
	}
	
	if (chan->isTopicRestricted() && !chan->isOperator(&client))
	{
		server.sendToClient(client.getFd(), ":ircserv 482 " + chanName + " :You're not channel operator\r\n");
		return ;
	}
	
	std::string newTopic = CommandHandler::getTrailing(params);
	chan->setTopic(newTopic);
	
	chan->broadcast(client.getPrefix() + " TOPIC " + chanName + " :" + newTopic + "\r\n");
}