/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParserHelpers.cpp                                 :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/11 12:18:27 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/11 12:25:50 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"

/*
* Helpers
*/
bool	tokenIsAlpha(t_config_token& token) {
	if (token.val.find_first_not_of("abcdefghijklmnopqrstuvwxyz_") != std::string::npos)
		return (false);
	return (true);
}

bool	pathIsValid(std::string path) { // add more checks here
	if (path.at(0) != '/')
		return (false);
	return (true);
}
/*
*/
