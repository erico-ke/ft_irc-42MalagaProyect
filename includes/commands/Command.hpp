/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 09:22:48 by fracurul          #+#    #+#             */
/*   Updated: 2026/05/28 09:22:51 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>

class	Client;
class	Server;

class	Command
{
	public:
		virtual	~Command() {}
		virtual void	execute(Client& client, const std::string& params, Server& server) = 0;
};

#endif
