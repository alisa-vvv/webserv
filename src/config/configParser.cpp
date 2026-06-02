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
// STATE 2 (lookState__BLOCK_PREFIX__BRACE_OPEN__:
// 			looking for BLOCK_PREFIX OR BRACE_OPEN -> STATE 3 -> STATE 2
// STATE 3 (lookState__KEY__BLOCK_NAME__CLOSE_BRACE__:
//			looking for KEY OR BLOCK_NAME OR BRACE_CLOSE -> STATE 1 -> STATE 2 -> STATE 4
// STATE 4 (lookState__VAL__:
// 			looking for VAL finished by SEMICOLON -> STATE 3

bool	tokenIsAlpha(t_config_token& token) {
	if (token.val.find_first_not_of("abcdefghijklmnopqrstuvwxyz_") != std::string::npos)
		return (false);
	return (true);
}

// STATE_1
e_state_label	lookState__BLOCK_NAME__(
	std::vector<t_config_token>& tokens,
	[[maybe_unused]] size_t	i,
	[[maybe_unused]] int& depth
) {
	e_state_label	next_state = STATE_2;
	t_config_token&	token = tokens.at(i);

	if (!tokenIsAlpha(token))
		return (STATE_ERR);
	token.type = BLOCK_NAME;
	return (next_state);
}

// STATE_2
e_state_label	lookState__BLOCK_PREFIX__BRACE_OPEN__(
	std::vector<t_config_token>& tokens,
	[[maybe_unused]] size_t	i,
	[[maybe_unused]] int& depth
) {
	e_state_label	next_state = STATE_2;
	t_config_token&	token = tokens.at(i);

	if (token.val == "{") {
		depth++;
		token.type = BRACE_OPEN;
		return (STATE_3);
	}
	token.type = BLOCK_PREFIX;
	return (next_state);
}

// STATE_3
// here, we need to look ahead.
// next token can be PREFIX or OPEN_BRACE. if it is, we're looking at block name.
// otherwise, we're looking at key.
// or we go to other state
e_state_label	lookState__KEY__BLOCK_NAME__CLOSE_BRACE__(
	std::vector<t_config_token>& tokens,
	[[maybe_unused]] size_t& i,
	[[maybe_unused]] int& depth
) {
	t_config_token&	cur_token = tokens.at(i);

	if (cur_token.val == "}") {
		depth--;
		cur_token.type = BRACE_CLOSE;
		return (STATE_3);
	}
	if (i + 1 < tokens.size()) {
		t_config_token&	next_token = tokens.at(i + 1);
		if (next_token.val == "{") {
			depth++;
			next_token.type = BRACE_OPEN;
			cur_token.type = BLOCK_NAME;
			return (STATE_3);
		}
		if (i + 2 < tokens.size()) {
			t_config_token&	next_next_token = tokens.at(i + 2);
			if (next_next_token.val == "{") {
				depth++;
				cur_token.type = BLOCK_NAME;
				next_token.type = BLOCK_PREFIX;
				next_next_token.type = BRACE_OPEN;
				return (STATE_3);
			}
		}
	}
	cur_token.type = KEY;
	return (STATE_4);
}

// STATE_4
e_state_label	lookState__VAL__(
	std::vector<t_config_token>& tokens,
	[[maybe_unused]] size_t	i,
	[[maybe_unused]] int& depth,
	bool& in_keyval
) {
	in_keyval = true;
	tokens.at(i).type = VALUE;
	if (tokens.at(i).val.back() != ';') {
		return (STATE_4);
	}
	in_keyval = false;
	return (STATE_3);
}

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
			if (!new_token.val.empty()) {
				new_token.type = UNDEFINED_TYPE;
				tokens.push_back(new_token);
			}
		}
		//new_token.val = '\n';
		//new_token.type = NEWLINE;
		//tokens.push_back(new_token);
	}

	return (tokens);
}

std::string	TEST_state_to_str(e_state_label	state) {
	switch (state) {
		case STATE_1:
			return ("STATE_1");
		case STATE_2:
			return ("STATE_2");
		case STATE_3:
			return ("STATE_3");
		case STATE_4:
			return ("STATE_4");
		case STATE_END:
			return ("STATE_END");
		case STATE_ERR:
			return ("STATE_ERR");
	}
}

void	TEST_print_one_token(t_config_token& token, size_t token_idx) {
		std::cout << "Tok " << token_idx << ".\n";
		std::cout << "\tTYPE:\t";
		std::cout << CLR_GRN;
		switch (token.type) {
			case UNDEFINED_TYPE:
				std::cout << CLR_RED <<  "UNDEFINED_TYPE" << CLR_GRN;
				break ;
			case BLOCK_NAME:
				std::cout << "BLOCK_NAME";
				break ;
			case BLOCK_PREFIX:
				std::cout << "BLOCK_PREFIX";
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
		std::cout << CLR_NON;
		std::cout << '\n';
		std::cout << "\tVAL:\t";
		std::cout << CLR_YEL;
		if (token.val == "\n") {
			std::cout << CLR_CYA << "\\n" << CLR_NON;
		}
		else
			std::cout << token.val;
		std::cout << CLR_NON;
		std::cout << '\n';
}

void	TEST_print_tokens(std::vector<t_config_token> tokens) {
	std::cout << "checking tokens, size: \n" << tokens.size() << '\n';
	for (size_t i = 0; i < tokens.size(); i++) {
		TEST_print_one_token(tokens.at(i), i);
	}

}

int	evaluateTokens(std::vector<t_config_token>& tokens) {
	int				depth = 0;
	bool			in_keyval = false;
	size_t			i = 0;
	e_state_label	cur_state = STATE_1;

	while (i < tokens.size()) {
		if (tokens.at(i).type != UNDEFINED_TYPE) {
			i++;
			continue ;
		}
		switch (cur_state) {
			case STATE_END:
				return (0);
			case STATE_ERR:
				return (1);
			case STATE_1:
				cur_state = lookState__BLOCK_NAME__(tokens, i, depth);
				std::cout << "evalled token: ";
				TEST_print_one_token(tokens.at(i), i);
				std::cout << "after STATE_1, returned: " << TEST_state_to_str(cur_state) << '\n';
				break ;
			case STATE_2:
				cur_state = lookState__BLOCK_PREFIX__BRACE_OPEN__(tokens, i, depth);
				std::cout << "evalled token: ";
				TEST_print_one_token(tokens.at(i), i);
				std::cout << "after STATE_2, returned: " << TEST_state_to_str(cur_state) << '\n';
				break ;
			case STATE_3:
				cur_state = lookState__KEY__BLOCK_NAME__CLOSE_BRACE__(tokens, i, depth);
				std::cout << "evalled token: ";
				TEST_print_one_token(tokens.at(i), i);
				std::cout << "after STATE_3, returned: " << TEST_state_to_str(cur_state) << '\n';
				break ;
			case STATE_4:
				cur_state = lookState__VAL__(tokens, i, depth, in_keyval);
				std::cout << "evalled token: ";
				TEST_print_one_token(tokens.at(i), i);
				std::cout << "after STATE_4, returned: " << TEST_state_to_str(cur_state) << '\n';
				break ;
		}
		i++;
	}
	if (in_keyval == true || depth > 0) {
		std::cout << CLR_RED << "Error! Incorrect configuration file:\n";
		if (in_keyval == true)
			std::cout << "\tMissing \";\" in key-value pair\n";
		if (depth > 0)
			std::cout << "\tMissing a closing brace in a block";
		return (1);
	}
	return (0);
}

int	parseConfig() {
	std::ifstream				config_file(CONFIG_PATH_TEST);
	std::vector<t_config_token>	tokens = tokenize(config_file);

	if (evaluateTokens(tokens) == 1)
		return (1);
	else
		TEST_print_tokens(tokens);

	return (0);
}
