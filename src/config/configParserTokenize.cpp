/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParserTokenize.cpp                                :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/03 14:49:56 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/03 15:57:45 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParserTEST.hpp"
#include "configParser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

// FINDING_BLOCK
static e_state_label	findBlock(
	std::vector<t_config_token>& tokens,
	size_t i,
	int& depth
) {
	t_config_token&	cur_token = tokens.at(i);

	if (cur_token.val == "}") {
		depth--;
		cur_token.type = BRACE_CLOSE;
		return (FINDING_BLOCK);
	}
	if (i + 1 < tokens.size()) {
		t_config_token&	next_token = tokens.at(i + 1);
		if (next_token.val == "{") {
			depth++;
			next_token.type = BRACE_OPEN;
			cur_token.type = BLOCK_NAME;
			return (FINDING_BLOCK);
		}
		if (i + 2 < tokens.size()) {
			t_config_token&	next_next_token = tokens.at(i + 2);
			if (next_next_token.val == "{") {
				depth++;
				cur_token.type = BLOCK_NAME;
				next_token.type = BLOCK_PREFIX;
				next_next_token.type = BRACE_OPEN;
				return (FINDING_BLOCK);
			}
		}
	}
	cur_token.type = KEY;
	return (FINDING_VALUES);
}

// FINDING_VALUES
static e_state_label	findValues(
	std::vector<t_config_token>& tokens,
	size_t	i,
	bool& in_keyval
) {
	in_keyval = true;
	tokens.at(i).type = VALUE;
	if (tokens.at(i).val.back() != ';') {
		return (FINDING_VALUES);
	}
	tokens.at(i).val.pop_back(); // remove ;
	in_keyval = false;
	return (FINDING_BLOCK);
}

std::vector<t_config_token>	tokenize(
	std::ifstream&	config_file
) {
	std::string					cur_line;
	std::vector<t_config_token>	tokens;
	t_config_token				new_token;
	int							line_number = 0;

	while (std::getline(config_file, cur_line)) {
		line_number++;
		std::istringstream	cur_line_stream(cur_line);

		while (cur_line_stream >> new_token.val) {
			if (new_token.val[0] == '#') {
				break ;
			}
			if (!new_token.val.empty()) {
				new_token.type = UNDEFINED_TYPE;
				new_token.line_number = line_number;
				tokens.push_back(new_token);
			}
		}
		//new_token.val = '\n';
		//new_token.type = NEWLINE;
		//tokens.push_back(new_token);
	}

	return (tokens);
}

static void	evalTokensError(int depth, bool in_keyval) {
	std::cout << CLR_RED << "Error! Incorrect configuration file:\n";
	if (in_keyval == true)
		std::cout << "\tMissing \";\" in key-value pair";
	if (depth > 0)
		std::cout << "\tMissing a closing brace in a block";
	if (depth < 0)
		std::cout << "\tMissing an opening brace in a block";
	std::cout << "\n";
}

int	evaluateTokens(std::vector<t_config_token>& tokens) {
	int				depth = 0;
	bool			in_keyval = false;
	e_state_label	cur_state = FINDING_BLOCK;

	for (size_t i = 0; i < tokens.size(); i++) {
		if (tokens.at(i).type != UNDEFINED_TYPE) {
			if (SHOW_CONFIG_PARSER_DEBUG == true) {
				std::cout << "skipping evalled token: ";
				TEST_print_one_token(tokens.at(i), i);
			}
			continue ;
		}
		else if (cur_state == FINDING_BLOCK) {
			cur_state = findBlock(tokens, i, depth);
			if (depth < 0) {
				evalTokensError(depth, in_keyval);
				return (1);
			}
			if (SHOW_CONFIG_PARSER_DEBUG == true) {
				std::cout << "evalled token: ";
				TEST_print_one_token(tokens.at(i), i);
				std::cout << "after FINDING_BLOCK, returned: " << TEST_state_to_str(cur_state) << '\n';
			}
		}
		else if (cur_state == FINDING_VALUES) {
			cur_state = findValues(tokens, i, in_keyval);
			if (SHOW_CONFIG_PARSER_DEBUG == true) {
				std::cout << "evalled token: ";
				TEST_print_one_token(tokens.at(i), i);
				std::cout << "after FINDING_VALUES, returned: " << TEST_state_to_str(cur_state) << '\n';
			}
		}
		// this else should never happen unless things are broken. remove later!
		else {
			std::cout << "breaking nwes: \n" << TEST_state_to_str(cur_state) << '\n';
			break ;
		}
	}
	if (in_keyval == true || depth != 0) {
		evalTokensError(depth, in_keyval);
		return (1);
	}
	return (0);
}
