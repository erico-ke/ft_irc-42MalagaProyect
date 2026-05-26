#ifndef NICKCOMMAND_HPP
# define NICKCOMMAND_HPP

# include "Command.hpp"

class	NickCommand : public Command
{
public:
	void	execute(Client& client, const std::string& params, Server& server);
};

#endif