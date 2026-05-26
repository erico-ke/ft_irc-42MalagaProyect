#ifndef INVITECOMMAND_HPP
# define INVITECOMMAND_HPP

# include "Command.hpp"

class InviteCommand : public Command
{
public:
	void execute(Client& client, const std::string& params, Server& server);
};

#endif
