/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserCommand.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 09:23:47 by fracurul          #+#    #+#             */
/*   Updated: 2026/05/28 15:21:43 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USERCOMMAND_HPP
# define USERCOMMAND_HPP

# include "Command.hpp"

class	UserCommand : public Command
{
	public:
		void	execute(Client& client, const std::string& params, Server& server);
};

#endif