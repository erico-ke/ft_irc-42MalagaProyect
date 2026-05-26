#ifndef QUITCOMMAND_HPP
# define QUITCOMMAND_HPP

# include "Command.hpp"

class	QuitCommand : public Command
{
public:
	void	execute(Client& client, const std::string& params, Server& server);
};

#endif