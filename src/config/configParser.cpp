/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParser.cpp                                        :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 13:13:56 by avaliull            #+#    #+#           */
/*   Updated: 2026/05/28 17:41:20 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

typedef enum	e_state_label {
	STATE_1,
	STATE_2,
	STATE_3,
	STATE_4,
	STATE_END,
	STATE_ERR,
}	e_state_label;
// STATE 1 (lookState__BLOCK_NAME__(): looking for BLOCK_NAME -> STATE 2
// STATE 2 (lookState__BLOCK_VAL__BRACE_OPEN__:
// 			looking for BLOCK_VAL OR BRACE_OPEN -> STATE 3 -> STATE 2
// STATE 3 (lookState__KEY__BLOCK_NAME__CLOSE_BRACE__:
//			looking for KEY OR BLOCK_NAME OR BRACE_CLOSE -> STATE 1 -> STATE 2 -> STATE 4
// STATE 4 (lookState__VAL__:
// 			looking for VAL finished by SEMICOLON -> STATE 3

e_state_label	lookState__BLOCK_NAME__(
	std::vector<t_config_token>& tokens,
	[[maybe_unused]] size_t	i,
	[[maybe_unused]] int& depth
) {
	e_state_label	next_state = STATE_2;
	t_config_token&	token = tokens.at(i);

	if (token.val.find_first_not_of("abcdefghijklmnopqrstuvwxyz_") != std::string::npos)
		return (STATE_ERR);
	token.type = BLOCK_NAME;
	return (next_state);
}

e_state_label	lookState__BLOCK_VAL__BRACE_OPEN__(
	std::vector<t_config_token>& tokens,
	[[maybe_unused]] size_t	i,
	[[maybe_unused]] int& depth
) {
	e_state_label	next_state = STATE_2;
	t_config_token&	token = tokens.at(i);

	if (token.val == "{") {
		token.type = BRACE_OPEN;
		return (STATE_3);
	}
	token.type = BLOCK_VAL;
	return (next_state);
}

//void	lookState__KEY__BLOCK_NAME__CLOSE_BRACE__(std::string token, int depth, int i) {
//}
//
//void	lookState__VAL__(std::string token, int depth, int i) {
//}
//
std::vector<t_config_token>	tokenize(
	std::ifstream&	config_file
) {
	std::string					cur_line;
	std::vector<t_config_token>	tokens;
	t_config_token				new_token;

	while (std::getline(config_file, cur_line)) {
		std::istringstream	cur_line_stream(cur_line);

		while (cur_line_stream >> new_token.val) {
			if (new_token.val[0] == '#') {
				break ;
			}
			if (!new_token.val.empty())
				tokens.push_back(new_token);
		}
		//new_token.val = '\n';
		//new_token.type = NEWLINE;
		//tokens.push_back(new_token);
	}

	return (tokens);
}

void	TEST_print_tokens(std::vector<t_config_token> tokens) {
	std::cout << "checking tokens, size: \n" << tokens.size() << '\n';
	for (size_t i = 0; i < tokens.size(); i++) {
		std::cout << "Tok " << i << ".\n";
		std::cout << "\tTYPE:\t";
		switch (tokens.at(i).type) {
			case BLOCK_NAME:
				std::cout << "BLOCK_NAME";
				break ;
			case BLOCK_VAL:
				std::cout << "BLOCK_VAL";
				break ;
			case BRACE_OPEN:
				std::cout << "BRACE_OPEN";
				break ;
			case BRACE_CLOSE:
				std::cout << "BRACE_CLOSE";
				break ;
			case KEY:
				std::cout << "KEY";
				break ;
			case VALUE:
				std::cout << "VALUE";
				break ;
			case SEMICOLON:
				std::cout << "SEMICOLON";
				break ;
			case NEWLINE:
				std::cout << "NEWLINE";
			break ;
		}
		std::cout << '\n';
		std::cout << "\tVAL:\t";
		if (tokens.at(i).val == "\n") {
			std::cout << CLR_CYA << "\\n" << CLR_NON;
		}
		else
			std::cout << tokens.at(i).val;
		std::cout << '\n';
	}

}

int	evaluateTokens(std::vector<t_config_token>& tokens) {
	int				depth = 0;
	size_t			i = 0;
	e_state_label	cur_state = STATE_1;

	while (i < tokens.size()) {
		switch (cur_state) {
			case STATE_END:
				return (0);
			case STATE_ERR:
				return (1);
			case STATE_1:
				cur_state = lookState__BLOCK_NAME__(tokens, i, depth);
				break ;
			case STATE_2:
				cur_state = lookState__BLOCK_VAL__BRACE_OPEN__(tokens.at(i), depth);
				break ;
			case STATE_3:
				;
				break ;
			case STATE_4:
				;
				break ;
		}
		i++;
	}

	return (0);
}

int	parseConfig() {
	std::ifstream				config_file(CONFIG_PATH_TEST);
	std::vector<t_config_token>	tokens = tokenize(config_file);
	evaluateTokens(tokens);
	TEST_print_tokens(tokens);

	return (0);
}
