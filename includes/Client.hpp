/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:53:41 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/13 14:29:03 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "./CodeUtils.hpp"
# include <string>

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
	Client(void){ throw invalidConstructorCall();}
	Client(int fd);
	Client(const Client &other) {(void)other; throw invalidConstructorCall();}
	Client &operator=(const Client &other) {(void)other; throw invalidConstructorCall();}
	~Client(void);
	
	int			getFd()			const;
	std::string	getNickname()	const;
	std::string	getUsername()	const;
	std::string	getRealname()	const;
	bool		isAuth()		const;
	bool		passGiven()		const;
	bool		nickGiven()		const;
	bool		userGiven()		const;

	void	setNickname(const std::string &nick);
	void	setUsername(const std::string &user);
	void	setRealname(const std::string &real);
	void	setAuth(bool val);
	void	setPassGiven(bool val);
	void	setNickGiven(bool val);
	void	setUserGiven(bool val);

	void		appendToBuffer(const std::string &data);
	std::string	&getBufferRef();

	std::string	getPrefix()	const;
};

#endif