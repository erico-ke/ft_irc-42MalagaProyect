/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeCommand.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 11:32:46 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/28 09:09:06 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODECOMMAND_HPP
# define MODECOMMAND_HPP

# include "Command.hpp"

class	ModeCommand : public Command
{
	public:
		void	execute(Client &client, const std::string &params, Server &server);
};

#endif