/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 11:34:01 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/27 19:13:34 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands/ModeCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/commands/CommandHandler.hpp"

void	ModeCommand::execute(Client &client, const std::string &params, Server &server)
{
	if (!client.isAuth()) return ;

	std::vector<std::string> args = CommandHandler::splitParams(params);
	if (args.empty())
	{
		server.sendToClient(client.getFd(), ":ircserv 461 MODE :Not enough parameters\r\n");
		return ;
	}
	
	std::string	target = args[0];
	std::string	modeStr = (args.size() > 1) ? args[1] : "";
	std::string	arg = (args.size() > 2) ? args[2] : "";

	if (target[0] != '#') return ;

	Channel	*chan = server.getChannel(target);
	if (!chan)
	{
		server.sendToClient(client.getFd(), ":ircserv 403 " + target + " :No such channel\r\n");
		return ;
	}
	
	if (modeStr.empty())
	{
		server.sendToClient(client.getFd(), ":ircserv 324 " + client.getNickname() + " " + target + " +\r\n");
		return ;
	}

	if (!chan->isOperator(&client))
	{
		server.sendToClient(client.getFd(), ":ircserv 482 " + target + " :You're not channel operator\r\n");
		return ;
	}
	
	bool	adding = true;
	std::string	appliedModes = "";
	std::string	appliedArgs = "";

	for (size_t i = 0; i < modeStr.size(); i++)
	{
		char	m = modeStr[i];

		if (m == '+') { adding = true; continue;}
		if (m == '-') { adding = false; continue;}

		switch (m)
		{
		case 'i':
			chan->setInviteOnly(adding);
			appliedModes += m;
			break ;
		case 't':
			chan->setTopicRestricted(adding);
			appliedModes += m;
			break ;
		case 'k':
			if (adding && arg.empty()) break ;
			chan->setKey(adding ? arg : "");
			appliedModes += m;
			if (adding) appliedArgs += " " + arg;
			break ;
		case 'l':
			if (adding)
			{
				if (arg.empty()) break ;
				int	limit = atoi(arg.c_str());
				if (limit > 0)
				{
					chan->setUserLimit(limit);
					appliedModes += m;
					appliedArgs += " " + arg;
				}
			}
			else
			{
				chan->setUserLimit(-1);
				appliedModes += m;
			}
			break ;
		case 'o':
			Client	*targetClient = server.getClientByNick(arg);
			if (targetClient->getNickname() == client.getNickname())
			{
				server.sendToClient(client.getFd(), ":ircserv 696 " + client.getNickname() + chan->getName() + " :You cannot target yourself with /mode -o\r\n");
				break ;
			}
			if (targetClient && chan->isMember(targetClient))
			{
				if (adding) chan->addOperator(targetClient);
				else chan->removeOperator(targetClient);
				appliedModes += m;
				appliedArgs += " " + arg;
			}
			break ;
		}
	}

	if (!appliedModes.empty())
	{
		std::string	modeChange = client.getPrefix() + " MODE " + target + " " + (adding ? "+" : "-") + appliedModes + appliedArgs + "\r\n";
		chan->broadcast(modeChange, server);
	}
}