#include "../../includes/commands/QuitCommand.hpp"
#include "../../includes/commands/CommandHandler.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"



void	QuitCommand::execute(Client &client, const std::string &params, Server &server)
{
	std::string	reason = CommandHandler::getTrailing(params);
	if (reason.empty()) reason = "Client quit";

	std::string	quitMsg = client.getPrefix() + " QUIT :" + reason + "\r\n";
	
	std::vector<Channel*>	channels = server.getChannelsForClient(&client);
	for (size_t i = 0; i < channels.size(); i++)
		channels[i]->broadcast(quitMsg, NULL);

	server.removeClient(client.getFd());
}