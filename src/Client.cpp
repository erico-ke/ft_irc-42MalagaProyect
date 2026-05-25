/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:37:35 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/21 21:54:17 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

// * OCF * //

Client::Client(void) { throw invalidConstructorCall(); }

Client::Client(int fd) : _fd(fd), _authenticated(false), _passGiven(false), _nickGiven(false), _userGiven(false)
{
	DEBUG_LOG("Client constructor called with fd:" << fd);
}

Client::Client(const Client &other) { (void)other; throw invalidConstructorCall(); }

Client	&Client::operator=(const Client &other) { (void)other; throw invalidConstructorCall(); return(*this); }

Client::~Client() { DEBUG_LOG("Client destructor called for fd: " << _fd); }

// * Setters * //

void	Client::setNickname(const std::string &nick) { _nickname = nick; }

void	Client::setUsername(const std::string &user) { _username = user; }

void	Client::setRealname(const std::string &real) { _realname = real; }

void	Client::setAuth(bool val) { _authenticated = val; }

void	Client::setPassGiven(bool val) { _passGiven = val; }

void	Client::setNickGiven(bool val) { _nickGiven = val; }

void	Client::setUserGiven(bool val) { _userGiven = val; }

void	Client::appendToBuffer(const std::string &data) { _buffer += data; }

// * Getters * //

int	Client::getFd() const { return(_fd); }

std::string	Client::getNickname() const { return(_nickname); }

std::string	Client::getUsername() const { return(_username); }

std::string	Client::getRealname() const { return(_realname); }

std::string	Client::getPrefix() const { return(std::string(":") + _nickname + "!" + _username + "@localhost"); }

std::string	&Client::getBufferRef() { return(_buffer); }

bool	Client::isAuth() const { return(_authenticated); }
bool	Client::passGiven() const { return(_passGiven); }
bool	Client::nickGiven() const { return(_nickGiven); }
bool	Client::userGiven() const { return(_userGiven); }
