/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WhoCommand.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 14:39:15 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/26 14:40:06 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WHOCOMMAND_HPP
# define WHOCOMMAND_HPP

# include "Command.hpp"

class WhoCommand : public Command
{
private:
	
public:
	void	execute(Client& client, const std::string& params, Server& server);
};

#endif