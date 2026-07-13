/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParserError.cpp                                   :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/11 12:14:42 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/12 18:16:06 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include <optional>
#include <iostream>
#include <string>

void	locationValidationError(
	const std::string& message,
	const size_t server_index,
	const size_t location_index
) {
	std::cout << "Config validation error: ";
	std::cout << message;
	std::cout << " from location block No. " << location_index;
	std::cout << " inside server block No. " << server_index;
}

void	displayParserError(
	const std::string& message,
	const std::optional<std::string>& prefix = std::nullopt
) {
	std::cout << CLR_RED;
	if (prefix)
		std::cout << *prefix << ": ";
	std::cout << message;
 	std::cout << CLR_NON << std::endl;
}

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
	displayParserError(message, prefix);
}
