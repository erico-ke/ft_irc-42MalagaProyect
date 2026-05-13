/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:39:24 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/13 14:29:46 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include <algorithm>
# include <iostream>
# include <sys/socket.h>

class Client;

class Channel
{
private:
	std::string				_name;
	std::string				_topic;
	std::string				_key;
	std::vector<Client*>	_members;
	std::vector<Client*>	_operators;
	std::vector<Client*>	_invited;
	bool					_inviteOnly;
	bool					_topicRestricted;
	int						_userLimit;

public:
	class invalidConstructorCall : public std::exception
	{
	public:
		const char *what() const throw();
	};
	Channel(void){throw Channel::invalidConstructorCall();}
	Channel(const std::string &name);
	Channel(const Channel &other) {(void)other; throw Channel::invalidConstructorCall();}
	Channel &operator=(const Channel &other) {(void)other; throw Channel::invalidConstructorCall();}
	~Channel(void);

	void	addMember(Client *client);
	void	removeMember(Client *client);
	bool	isMember(Client *client)		const;
	bool	isEmpty()						const;

	void	addOperator(Client *client);
	void	removeOperator(Client *client);
	bool	isOperator(Client *client) const;

	void	addInvite(Client *client);
	bool	isInvited(Client *client) const;

	void	broadcast(const std::string &msg, Client *exept = NULL);

	std::string	getName()			const;
    std::string	getTopic()			const;
    std::string	getKey()			const;
    bool		isInviteOnly()		const;
    bool		isTopicRestricted()	const;
    int			getUserLimit()		const;
    size_t		getMemberCount()	const;
	
	void	setTopic(const std::string &topic);
	void	setKey(const std::string &key);
	void	setinviteOnly(bool val);
	void	setTopicRestricted(bool val);
	void	setUserLimit(int limit);
	
};

#endif