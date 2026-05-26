#ifndef PRIVMSGCOMMAND_HPP
# define PRIVMSGCOMMAND_HPP

# include "Command.hpp"

class	PrivmsgCommand : public Command
{
public:
	void	execute(Client& client, const std::string& params, Server& server);
};

#endif
