/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PassCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 10:01:36 by fracurul          #+#    #+#             */
/*   Updated: 2026/05/28 11:25:21 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands/PassCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"

void	PassCommand::execute(Client& client, const std::string& params, Server& server)
{
	if (client.isAuth())
	{
		server.sendToClient(client.getFd(), ":ircserv 462 " + client.getNickname() + " :You may not reregister\r\n");
		return ;
	}
	if (params.empty())
	{
		server.sendToClient(client.getFd(), ":ircserv 461 PASS :Not enough parameters\r\n");
		return ;
	}
	if (params != server.getPassword())
	{
		server.sendToClient(client.getFd(), ":ircserv 464 * :Password incorrect\r\n");
		return ;
	}
	client.setPassGiven(true);
}
