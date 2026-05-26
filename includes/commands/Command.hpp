#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>

class Client;
class Server;

class Command
{
public:
	virtual ~Command() {}
	virtual void execute(Client& client, const std::string& params, Server& server) = 0;
};

#endif
