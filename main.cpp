/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 12:20:39 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/12 13:45:29 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"
#include <iostream>
#include <cstdlib>

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << ERROR_COLOR << "Error, invalid input." << RESET << " Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	//Agregar parseo input A.K.A revisar que sea un numero el primero 
	//y que el segundo sea un password con sentido, caparle idioteces
	int port = atoi(argv[1]);
	if (port <= 1024 || port > 65535)
	{
		std::cerr << ERROR_COLOR << "Error, invalid port." << RESET << " A valid port is between 1024 and 65535," << std::endl;
		return 1;
	}
	
	std::string	password = argv[2];
	if (password.empty())
	{
		std::cerr << ERROR_COLOR << "Error, empty password." << RESET << std::endl;
		return 1;
	}
	
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