/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 12:20:39 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/07 12:27:12 by erico-ke         ###   ########.fr       */
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
	argv = 0;
}