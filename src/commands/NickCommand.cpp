#include "../../includes/commands/NickCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/commands/CommandHandler.hpp"
#include <sstream>
#include <algorithm>

void	NickCommand::execute(Client& client, const std::string& params, Server& server)
{
	if (params.empty())
	{
		server.sendToClient(client.getFd(), ":ircserv 431 * :No nickname given\r\n");
		return ;
	}

	Client*	nickUsed = server.getClientByNick(params);
	if (nickUsed && nickUsed->getFd() != client.getFd())
	{
		server.sendToClient(client.getFd(), ":ircserv 433 * " + params + " :Nickname already in use\r\n");
		return ;
	}

	std::string	oldNick = client.getNickname();
	client.setNickname(params);
	client.setNickGiven(true);
	if (client.isAuth())
	{
		server.sendToClient(client.getFd(), ":" + oldNick + "!user@localhost NICK :" + params + "\r\n");
		return ;
	}
	CommandHandler::_tryFinishAuth(client, server);
}