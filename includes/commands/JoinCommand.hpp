#ifndef JOINCOMMAND_HPP
# define JOINCOMMAND_HPP

# include "Command.hpp"

class JoinCommand : public Command
{
public:
	void execute(Client& client, const std::string& params, Server& server);
};

#endif
