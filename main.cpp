/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 12:20:39 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/26 13:00:44 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>

bool g_running = true;

void	signalHandler(int sig)
{
	(void)sig;
	g_running = false;
}

bool	portInputParser(char *arg)
{
	int i = 0;
	while (arg[++i])
	{
		if (arg[i] < '0' || arg[i] > '9')
			return false;
	}
	return true;
}

bool	parsePassword(const char *arg, std::string &out)
{
	out.clear();
	for (size_t i = 0; arg[i]; ++i)
	{
		unsigned char c = static_cast<unsigned char>(arg[i]);
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f')
			continue;
		if (c < 33 || c > 126)
			return false;
		out.push_back(static_cast<char>(c));
	}
	return !out.empty();
}

int	main(int argc, char **argv)
{
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGPIPE, signalHandler);

	if (argc != 3)
	{
		std::cerr << ERROR_COLOR << "Error, invalid input." << RESET << " Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	if (portInputParser(argv[1]) == false)
	{
		std::cerr << ERROR_COLOR << "Error, port input must be numeric." << RESET << std::endl;
		return 1;
	}
	DEBUG_LOG("Input validated");
	int port = atoi(argv[1]);
	if (port <= 1024 || port > 65535)
	{
		std::cerr << ERROR_COLOR << "Error, invalid port." << RESET << " A valid port is between 1024 and 65535," << std::endl;
		return 1;
	}
	DEBUG_LOG("Port " << port << " is valid");
	std::string	password;
	if (!parsePassword(argv[2], password))
	{
		std::cerr << ERROR_COLOR << "Error, invalid password." << RESET << std::endl;
		return 1;
	}
	DEBUG_LOG("Password " << password << "is valid");
	try
	{
		Server server(port, password);
		server.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << ERROR_COLOR << "Error: " << RESET << e.what() << std::endl;
		return 1;
	}

	return 0;
}