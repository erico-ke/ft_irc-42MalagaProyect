/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TopicCommand.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 10:51:58 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/28 09:09:15 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOPICCOMMAND_HPP
# define TOPICCOMMAND_HPP

# include "Command.hpp"

class	TopicCommand : public Command
{
	public:
		void	execute(Client &client, const std::string &params, Server &server);
};

#endif