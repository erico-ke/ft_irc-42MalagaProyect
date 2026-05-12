/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erico-ke <erico-ke@42malaga.student.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:25:10 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/12 13:47:07 by erico-ke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"

const char	*Server::invalidConstructorCall::what() const throw()
{
	return ("Invalid constructor call");
}

