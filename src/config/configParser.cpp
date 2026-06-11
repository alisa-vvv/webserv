/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParser.cpp                                        :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 13:13:56 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/11 12:31:49 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */
#include "configParser.hpp"
#include "configParserTEST.hpp"
#include "configParsingInfo.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <optional>
#include <arpa/inet.h>

void	matchTokenToContext(
	const t_config_token& token,
	std::stack<e_context>& context
) {
	if (context.top() == GLOBAL) {
		if (token.val == "server") {
			context.push(SERVER);
		}
	}
	else if (context.top() == SERVER) {
		if (token.val == "location")
			context.push(LOCATION);
	}
}

tokenParserFnPtr_t	matchTokenValueToParser(
	const std::vector<std::string> allowed_strings,
	const std::vector<tokenParserFnPtr_t> parsers,
	const t_config_token& token
) {
	for (size_t i = 0; i < allowed_strings.size(); i++) {
		if (i >= parsers.size() || parsers.size() == 0) { // this is for DEBUG only
			std::cout << CLR_RED << "ERROR: Parser not defined for " << token.val;
			std::cout << CLR_NON << '\n';
			return (NULL);
		}
		if (token.val == allowed_strings.at(i)) {
	  		return (parsers.at(i));
		}
	}
	return (NULL);
}

tokenParserFnPtr_t	matchTokenValueToParserAccordingToContext(
	const ParsingInfo parsing_info,
	const t_config_token& token,
	std::stack<e_context>& context_stack
) {
	std::vector<std::string>		allowed_strings;
	std::vector<tokenParserFnPtr_t>	parsers;

	if (context_stack.top() == GLOBAL) {
		allowed_strings = parsing_info.depth0_valid_block_names;
		parsers = parsing_info.depth_0_matching_functions;
	}
	if (context_stack.top() == SERVER) {
		allowed_strings = parsing_info.server_valid_block_names;
		parsers = parsing_info.server_matching_functions;
	}
	return (matchTokenValueToParser(allowed_strings, parsers, token));
}

// step 0: set context to GLOBAL.
// step 1: set cur_token to be token.at(i)
// step 2:
// 	if (type == BLOCK_NAME)
		// step 1: match block type to context, change context stack if needed
		// step 2: using context, match block name with parser func
		// step 3: perform the appropriate function
// 		// step 4: pop a context from stack
// else if (type == 
// 
Config	tokensToConfig(
	const ParsingInfo parsing_info,
	std::vector<t_config_token>& tokens
) {
	Config					config;
	std::stack<e_context>	context_stack;
	tokenParserFnPtr_t		parser = NULL;

	context_stack.push(GLOBAL);
	for (size_t i = 0; i < tokens.size(); i++) {
		if (SHOW_CONFIG_PARSER_DEBUG == true)
			std::cout << "Converting token No." << i << '\n';
		const t_config_token&	cur_token = tokens.at(i);
		if (cur_token.type == EVALUATED || cur_token.type == BRACE_OPEN
			|| cur_token.type == BRACE_CLOSE) {
			continue ;
		}
		if (cur_token.type == BLOCK_NAME || cur_token.type == KEY) {
			parser = matchTokenValueToParserAccordingToContext(
				parsing_info, cur_token, context_stack);
			if (parser == NULL) {
				std::cout << CLR_RED << "SOMETHING SOMETHING ERROR\n" << CLR_NON;
				return (config);
			}
			else {
				(*parser)(config, parsing_info, i, tokens);
				matchTokenToContext(cur_token, context_stack);
	  		}
		}
		else if (cur_token.type == BLOCK_PREFIX) {
			// tbd
		}
	}
	return (config);
}

int	parseConfig(const ParsingInfo parsing_info) {
	std::ifstream				config_file(CONFIG_PATH_TEST);
	std::vector<t_config_token>	tokens;

	tokens = tokenize(config_file);
	if (evaluateTokens(tokens) == 1)
		return (1);
	else
		TEST_print_tokens(tokens);

	Config config = tokensToConfig(parsing_info, tokens);
	return (0);
}
