/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParser.cpp                                        :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 13:13:56 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/12 18:50:02 by avaliull            ########   odam.nl   */
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
		allowed_strings = parsing_info.global_valid_block_names;
		parsers = parsing_info.global_matching_functions;
	}
	if (context_stack.top() == SERVER) {
		allowed_strings = parsing_info.server_valid_block_names;
		parsers = parsing_info.server_matching_functions;
	}
	if (context_stack.top() == LOCATION) {
		allowed_strings = parsing_info.location_valid_block_names;
		parsers = parsing_info.location_matching_functions;
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
	t_config_token&	cur_token = tokens.at(0);
	for (size_t i = 0; i < tokens.size(); i++) {
		cur_token = tokens.at(i);
		if (cur_token.type == EVALUATED || cur_token.type == BRACE_OPEN) {
			if (SHOW_CONFIG_PARSER_DEBUG == true) {
				std::cout << "Skipping token No." << i << ": ";
				std::cout << cur_token.val << '\n';
			}
			continue ;
		}
		else if (cur_token.type == BRACE_CLOSE) {
			context_stack.pop();
			if (SHOW_CONFIG_PARSER_DEBUG == true) {
				std::cout << "Skipping close brace token No." << i << ": ";
				std::cout << cur_token.val << '\n';
			}
		}
		else if (cur_token.type == BLOCK_NAME || cur_token.type == KEY) {
			if (SHOW_CONFIG_PARSER_DEBUG == true) {
				std::cout << "Converting token No." << i << ": ";
				std::cout << cur_token.val << '\n';
			}
			parser = matchTokenValueToParserAccordingToContext(
				parsing_info, cur_token, context_stack);
			if (parser == NULL) {
				std::cout << CLR_RED << "SOMETHING SOMETHING ERROR\n" << CLR_NON;
				return (config);
			}
			else {
				if ((*parser)(config, i, tokens) == false) {
					return (config);
				}
				matchTokenToContext(cur_token, context_stack);
	  		}
		}
		else {
			std::cout << CLR_RED << "ERROR: TOKEN TYPE NOT COVERED BY IFELSE\n" << CLR_NON;
			return (config);
		}
	}
	for (size_t i = 0; i < tokens.size(); i++) {
		cur_token = tokens.at(i);
		if (cur_token.type != EVALUATED && cur_token.type != BRACE_OPEN
			&& cur_token.type != BRACE_CLOSE) {
			std::cout << CLR_RED << "ERROR: TOKEN No." << i; 
			std::cout << " on line " << cur_token.line_number;
			std::cout << " NOT EVALUATED" << CLR_NON << '\n';
			return (config);
		}
	}
	return (config);
}

//typedef struct t_location {
//	std::string					prefix; // needed
//	std::string					root; // needed
//	std::string					index; // opt if cgi_pass is not set
//	bool						autoindex; // needed
//	std::map<e_method, bool>	allowed_methods { {GET, false}, {POST, false}, {DELETE, false} }; // needed
//	std::string					upload_store; // opt
//	t_cgi_pass					cgi_pass; // opt
//	t_return					returns; // opt
//}	t_location;

bool	locationIsValid( // move this for when we pop context stack?
	const t_location& location,
	const size_t server_index,
	const size_t location_index
) {
	if (location.prefix.size() == 0) {
		locationValidationError("missing prefix", server_index, location_index);
		return (false);
	}
	if (location.root.size() == 0) {
		locationValidationError("missing root", server_index, location_index);
		return (false);
	}
	// this is sketchy, double check what's going on here
	if (location.index.size() == 0 && location.cgi_pass.path.size() == 0) {
		locationValidationError("missing index", server_index, location_index);
		return (false);
	}
	return (true);
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
