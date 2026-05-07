/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:53:41 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/07 16:59:11 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

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
class Client
{
private:
	int			_fd;
	std::string	_nickname;
	std::string	_username;
	std::string	_realname;
	std::string	_buffer;
	bool		_authenticated;
	bool		_passGiven;
	bool		_nickGiven;
	bool		_userGiven;
public:
	Client(void);
	Client(int fd);
	Client(const Client &other);
	Client &operator=(const Client &other);
	~Client(void);
	
	
};

#endif