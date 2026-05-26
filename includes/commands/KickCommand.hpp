#ifndef KICKCOMMAND_HPP
# define KICKCOMMAND_HPP

# include "Command.hpp"

class KickCommand : public Command
{
public:
	void execute(Client& client, const std::string& params, Server& server);
};

#endif
