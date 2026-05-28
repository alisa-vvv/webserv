/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParser.cpp                                        :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 13:13:56 by avaliull            #+#    #+#           */
/*   Updated: 2026/05/28 16:17:53 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

// STATE 1: looking for BLOCK_NAME -> STATE 2
// STATE 2: looking for BLOCK_VALUE OR open brace -> STATE 3
// STATE 3: looking for KEY OR close brace -> STATE 2 -> STATE 4
// STATE 4: looking for VALUE finished by SEMICOLON
int	parseConfig() {
	std::ifstream				config_file(CONFIG_PATH_TEST);
	std::string					cur_line;
	std::vector<t_config_token>	tokens;
	t_config_token				new_token;

	while (std::getline(config_file, cur_line)) {
		std::istringstream	cur_line_stream(cur_line);

		while (cur_line_stream >> new_token.token_val) {
			if (new_token.token_val[0] == '#') {
				break ;
			}
			if (!new_token.token_val.empty())
				tokens.push_back(new_token);
		}
		new_token.token_val = '\n';
		new_token.token_type = NEWLINE;
		tokens.push_back(new_token);
	}

	std::cout << "checking tokens, size: \n" << tokens.size() << '\n';
	for (size_t i = 0; i < tokens.size(); i++) {
		std::cout << "(";
		if (tokens.at(i).token_val == "\n") {
			std::cout << CLR_CYA << "\\n" << CLR_NON;
		}
		else
			std::cout << tokens.at(i).token_val;
		std::cout << ")";
	}
	return (0);
}
