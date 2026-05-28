/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KickCommand.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 09:23:11 by fracurul          #+#    #+#             */
/*   Updated: 2026/05/28 09:23:12 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KICKCOMMAND_HPP
# define KICKCOMMAND_HPP

#include "Command.hpp"

class	KickCommand : public Command
{
	public:
		void	execute(Client& client, const std::string& params, Server& server);
};

#endif
