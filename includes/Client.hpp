/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:53:41 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/28 11:20:45 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "./CodeUtils.hpp"
# include <string>

class	Client
{
	private:
		int			_fd;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;
		std::string	_buffer;
		std::string	_outBuffer;
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

		// * Getters * //
		int			getFd()			const;
		std::string	getNickname()	const;
		std::string	getUsername()	const;
		std::string	getRealname()	const;
		std::string	getPrefix()	const;
		std::string	&getBufferRef();
		std::string	&getOutBufferRef();
		bool		isAuth()		const;
		bool		passGiven()		const;
		bool		nickGiven()		const;
		bool		userGiven()		const;

		// * Setters * //
		void	setNickname(const std::string &nick);
		void	setUsername(const std::string &user);
		void	setRealname(const std::string &real);
		void	setAuth(bool val);
		void	setPassGiven(bool val);
		void	setNickGiven(bool val);
		void	setUserGiven(bool val);

		// * Helper methods * //
		void		appendToBuffer(const std::string &data);
		void		appendToOutBuffer(const std::string &data);
};

#endif