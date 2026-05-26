#include "../../includes/commands/UserCommand.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"
#include "../../includes/commands/CommandHandler.hpp"

void	UserCommand::execute(Client& client, const std::string& params, Server& server)
{
	if (client.isAuth())
	{
		server.sendToClient(client.getFd(), ":ircserv 462 " + client.getNickname() + " :You may not reregister\r\n");
		return;
	}
	std::vector<std::string>	args = CommandHandler::splitParams(params);
	if (args.size() < 1)
	{
		server.sendToClient(client.getFd(), ":ircserv 461 USER :Not enough parameters\r\n");
		return;
	}
	client.setUsername(args[0]);
	client.setRealname(CommandHandler::getTrailing(params));
	client.setUserGiven(true);
	CommandHandler::_tryFinishAuth(client, server);
}
