/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QuitCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 10:01:27 by fracurul          #+#    #+#             */
/*   Updated: 2026/05/28 11:26:02 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands/QuitCommand.hpp"
#include "../../includes/commands/CommandHandler.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"

void	QuitCommand::execute(Client &client, const std::string &params, Server &server)
{
	if (!client.isAuth()) { return ; }
	std::string	reason = CommandHandler::getTrailing(params);
	if (reason.empty()) reason = "Client quit";

	std::string				quitMsg = client.getPrefix() + " QUIT :" + reason + "\r\n";
	std::vector<Channel *>	channels = server.getChannelsForClient(&client);
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i]->isOperator(&client))
		{
			const std::vector<Client *>	members = channels[i]->getMembers();
			int							count = 0;
			for (size_t j = 0; j < members.size(); j++)
				if (channels[i]->isOperator(members[j]) && members[j]->getFd() != client.getFd()) { count = 1; }
			if (count == 0)
			{
				Client	*candidate = NULL;
				for (size_t j = 0; j < members.size(); j++)
					if (members[j]->getFd() != client.getFd()) { candidate = members[j]; break ; }
				if (candidate) { channels[i]->addOperator(candidate); }
			}
		}
		channels[i]->broadcast(quitMsg, server, &client);
	}
	server.removeClient(client.getFd());
}