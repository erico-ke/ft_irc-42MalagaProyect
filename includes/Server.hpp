/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 12:22:34 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/07 12:23:39 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>

/* ===== DEBUG MODE ===== */
# ifndef DEBUG
#  define DEBUG 0
# endif

# if DEBUG == 1
#  define DEBUG_LOG(msg) do { std::cout << msg << std::endl; } while (0)
# else
#  define DEBUG_LOG(msg) do {} while (0)
# endif

/* ===== ANSI COLOR CODES ===== */
# define RESET "\033[0m"
# define CONSTRUCTOR_COLOR "\033[36m"
# define DESTRUCTOR_COLOR "\033[35m"
# define ERROR_COLOR "\033[91m"

/* ===== MAIN CLASS ===== */
class Server
{
private:
	
public:
	Server(void);
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server(void);
	
	
};

#endif