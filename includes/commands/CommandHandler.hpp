#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

# include "../Client.hpp"
# include "../Channel.hpp"
# include "../CodeUtils.hpp"
# include <string>
# include <vector>
# include <iostream>
# include <sstream>
# include <algorithm>
# include <cctype>

class Server;

/* ===== MAIN CLASS ===== */
class CommandHandler
{
private:
	static void	handleNick(Client &client, const std::string &params, Server &server);
	static void	handleUser(Client &client, const std::string &params, Server &server);
	static void	handleTopic(Client &client, const std::string &params, Server &server);
	static void	handleMode(Client &client, const std::string &params, Server &server);
	static void	handleQuit(Client &client, const std::string &params, Server &server);

public:
	CommandHandler(void) {throw invalidConstructorCall();}
	CommandHandler(const CommandHandler &other) {(void)other; throw invalidConstructorCall();}
	CommandHandler &operator=(const CommandHandler &other) {(void)other; throw invalidConstructorCall();}
	~CommandHandler(void);

	// Handler orchestor for Command classes
	static void	handle(Client &client, const std::string &line, Server &server);

	// Utility methods for parsing (used by Command classes)
	static std::string				toUpper(const std::string &s);
	static std::vector<std::string>	splitParams(const std::string &params);
	static std::string				getTrailing(const std::string &params);
	static void						_tryFinishAuth(Client &client, Server &server);
};

#endif
