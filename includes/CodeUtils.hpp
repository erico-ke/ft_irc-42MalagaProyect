/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CodeUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fracurul <fracurul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 10:51:18 by erico-ke          #+#    #+#             */
/*   Updated: 2026/05/28 12:21:23 by fracurul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEUTILS_HPP
# define CODEUTILS_HPP

# include <exception>

// * DEBUG MODE * //

# ifndef DEBUG
#  define DEBUG 0
# endif

# if DEBUG == 1
#  define DEBUG_LOG(msg) do { std::cout << msg << std::endl; } while (0)
# else
#  define DEBUG_LOG(msg) do {} while (0)
# endif

// * ANSI COLOR CODES * //
# define RESET "\033[0m"
# define CONSTRUCTOR_COLOR "\033[36m"
# define DESTRUCTOR_COLOR "\033[35m"
# define ERROR_COLOR "\033[91m"

// * GENERIC EXCEPTIONS * //
class	invalidConstructorCall : public std::exception
{
	public:
		const char *what() const throw() { return "Invalid constructor call"; }
};

#endif