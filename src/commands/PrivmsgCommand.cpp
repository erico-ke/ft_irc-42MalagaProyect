#include "../../includes/commands/PrivmsgCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/commands/CommandHandler.hpp"
#include <sstream>
#include <algorithm>

void	PrivmsgCommand::execute(Client& client, const std::string& params, Server& server)
{
	if (!client.isAuth()) return ;

	std::vector<std::string> args = CommandHandler::splitParams(params);
	std::string	message = CommandHandler::getTrailing(params);

	if (args.empty() || message.empty())
	{
		server.sendToClient(client.getFd(), ":ircserv 461 PRIVMSG :Not enough parameters\r\n");
		return ;
	}

	std::string	target = args[0];
	std::string	fullMsg = client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";

	if (target[0] == '#')
	{
		Channel	*chan = server.getChannel(target);
		if (!chan)
		{
			server.sendToClient(client.getFd(), ":ircserv 403 " + target + " :No such channel\r\n");
			return ;
		}
		if (!chan->isMember(&client))
		{
			server.sendToClient(client.getFd(), ":ircserv 404 " + target + " :Cannot send to channel (not a member)\r\n");
			return ;
		}
		chan->broadcast(fullMsg, &client);
	}
	else
	{
		Client	*dest = server.getClientByNick(target);
		if (!dest)
		{
			server.sendToClient(client.getFd(), ":ircserv 401 " + target + " :No such nick\r\n");
			return ;
		}
		server.sendToClient(dest->getFd(), fullMsg);
	}
}
