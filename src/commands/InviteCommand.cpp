#include "../../includes/commands/InviteCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/commands/CommandHandler.hpp"
#include <sstream>
#include <algorithm>

void	InviteCommand::execute(Client& client, const std::string& params, Server& server)
{
	if (!client.isAuth()) return;

	std::vector<std::string> args = CommandHandler::splitParams(params);
	if (args.size() < 2)
	{
		server.sendToClient(client.getFd(), ":ircserv 461 INVITE :Not enough parameters\r\n");
		return ;
	}

	std::string targetNick = args[0];
	std::string chanName   = args[1];

	Channel*	chan = server.getChannel(chanName);
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
	if (chan->isInviteOnly() && !chan->isOperator(&client))
	{
		server.sendToClient(client.getFd(), ":ircserv 482 " + chanName + " :You're not channel operator\r\n");
		return ;
	}

	Client*	target = server.getClientByNick(targetNick);
	if (!target)
	{
		server.sendToClient(client.getFd(), ":ircserv 401 " + targetNick + " :No such nick\r\n");
		return ;
	}

	chan->addInvite(target);

	server.sendToClient(target->getFd(), client.getPrefix() + " INVITE " + targetNick + " :" + chanName + "\r\n");

	server.sendToClient(client.getFd(), ":ircserv 341 " + client.getNickname() + " " + targetNick + " " + chanName + "\r\n");
}
