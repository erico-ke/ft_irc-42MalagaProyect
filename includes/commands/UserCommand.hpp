#ifndef USERCOMMAND_HPP
# define USERCOMMAND_HPP

# include "Command.hpp"

class	UserCommand : public Command
{
public:
	void	execute(Client& client, const std::string& params, Server& server);
};

#endif