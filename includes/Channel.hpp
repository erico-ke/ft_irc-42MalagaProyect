/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:39:24 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/12 12:44:49 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

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
	Channel(void);
	Channel(const std::string &name);
	Channel(const Channel &other);
	Channel &operator=(const Channel &other);
	~Channel(void);

	void	addMember(Client *client);
	void	removeMember(Client *client);
	bool	isMember(Client *client)		const;
	bool	isEmpty()						const;

	
	
	
};

#endif