/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:37:41 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/14 16:15:55 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"

Channel::Channel(const std::string &name) : _name(name), _inviteOnly(false), _topicRestricted(false), _userLimit(-1) {DEBUG_LOG("Channel constructor called");}

Channel::~Channel() {DEBUG_LOG("Channel destructor called");}

void	Channel::addMember(Client *client)
{
	if (!isMember(client))
		_members.push_back(client);
	else
		std::cout << client->getUsername() << " is already a member of channel " << _name << std::endl; 
}

void	Channel::removeMember(Client *client)
{
	_members.erase(std::remove(_members.begin(), _members.end(), client), _members.end());
	_operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
	_invited.erase(std::remove(_invited.begin(), _invited.end(), client), _invited.end());
}

bool	Channel::isMember(Client *client) const
{
	return std::find(_members.begin(), _members.end(), client) != _members.end();
}

bool	Channel::isEmpty() const { return _members.empty();}

void	Channel::addOperator(Client *client)
{
	if (!isOperator(client))
		_operators.push_back(client);
	else
		std::cout << client->getUsername() << "is already an operator in channel " << _name << std::endl;
}

void	Channel::removeOperator(Client *client)
{
	_operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

bool	Channel::isOperator(Client *client) const
{
	return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

void	Channel::addInvite(Client *client)
{
	if (!isInvited(client))
		_invited.push_back(client);
	else
		std::cout << client->getUsername() << " is already invited to channel " << _name << std::endl;
}

bool	Channel::isInvited(Client *client) const
{
	return std::find(_invited.begin(), _invited.end(), client) != _invited.end();
}

void	Channel::broadcast(const std::string &msg, Client *exept)
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] != exept)
			send(_members[i]->getFd(), msg.c_str(), msg.size(), 0);
	}
}