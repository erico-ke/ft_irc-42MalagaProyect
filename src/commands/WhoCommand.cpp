/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WhoCommand.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 14:44:45 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/26 14:45:57 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands/WhoCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/commands/CommandHandler.hpp"

void	WhoCommand::execute(Client& client, const std::string& params, Server& server)
{
	(void)client;
	(void)params;
	(void)server;
}