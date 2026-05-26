#ifndef PASSCOMMAND_HPP
# define PASSCOMMAND_HPP

# include "Command.hpp"

class	PassCommand : public Command
{
public:
	void	execute(Client& client, const std::string& params, Server& server);
};

#endif