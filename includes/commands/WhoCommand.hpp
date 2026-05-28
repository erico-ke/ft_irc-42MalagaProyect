/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WhoCommand.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 14:39:15 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/28 09:09:20 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WHOCOMMAND_HPP
# define WHOCOMMAND_HPP

# include "Command.hpp"

class	WhoCommand : public Command
{
	public:
		void	execute(Client& client, const std::string& params, Server& server);
};

#endif