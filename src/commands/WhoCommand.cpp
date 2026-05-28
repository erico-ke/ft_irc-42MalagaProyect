/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WhoCommand.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 14:44:45 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/28 11:30:13 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands/WhoCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/commands/CommandHandler.hpp"

void	WhoCommand::execute(Client& client, const std::string& params, Server& server)
{
	if (!client.isAuth()) { return ; }

	std::vector<std::string>	args = CommandHandler::splitParams(params);
	if (args.empty())
	{
		server.sendToClient(client.getFd(), ":irscerv 461 WHO :Not enough parameters\r\n");
		return ;
	}

	std::string	chanName = args[0];
	Channel		*chan = server.getChannel(chanName);
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

	const std::vector<Client *>	&members = chan->getMembers();
	for (size_t i = 0; i < members.size(); ++i)
	{
		Client		*member = members[i];
		std::string	nick = member->getNickname();
		if (chan->isOperator(member)) { nick = "@" + nick; }
		std::string	reply = ":ircserv 353 " + client.getNickname() + " = " + chanName + " :" + nick + "\r\n";
		server.sendToClient(client.getFd(), reply);
	}
	server.sendToClient(client.getFd(), ":ircserv 366 " + client.getNickname() + " " + chanName + " :End of /NAMES list\r\n");
}