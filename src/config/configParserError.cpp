/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParserError.cpp                                   :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/11 12:14:42 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/11 12:16:46 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include <optional>
#include <iostream>
#include <string>

/*
 * configParserError
 */
void	configParserError(
	Config& config,
	const std::string& message,
	const std::optional<std::string>& prefix = std::nullopt,
	const std::optional<int> line_number = std::nullopt
) {

	config.is_correct = false;
	if (line_number)
		std::cout << "Line " << *line_number << ": ";
	if (prefix)
		std::cout << *prefix << ": ";
	std::cout << message;
 	std::cout << std::endl;
}
