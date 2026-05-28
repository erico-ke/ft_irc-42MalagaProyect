/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 10:01:17 by fracurul          #+#    #+#             */
/*   Updated: 2026/05/28 10:01:18 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands/UserCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/commands/CommandHandler.hpp"

void	UserCommand::execute(Client& client, const std::string& params, Server& server)
{
	if (client.isAuth())
	{
		server.sendToClient(client.getFd(), ":ircserv 462 " + client.getNickname() + " :You may not reregister\r\n");
		return ;
	}

	std::vector<std::string>	args = CommandHandler::splitParams(params);
	if (args.size() < 1)
	{
		server.sendToClient(client.getFd(), ":ircserv 461 USER :Not enough parameters\r\n");
		return ;
	}
	if (!client.nickGiven())
	{
		server.sendToClient(client.getFd(), ":ircserv 431 :Expected NICK\r\n");
		return ;
	}
	client.setUsername(args[0]);
	client.setRealname(CommandHandler::getTrailing(params));
	client.setUserGiven(true);
	CommandHandler::_tryFinishAuth(client, server);
}
