/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParser.cpp                                        :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 13:13:56 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/03 15:50:42 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include "configParserTEST.hpp"
#include "configParsingInfo.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>

bool	tokenIsAlpha(t_config_token& token) {
	if (token.val.find_first_not_of("abcdefghijklmnopqrstuvwxyz_") != std::string::npos)
		return (false);
	return (true);
}


bool fillListenField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& block_start_idx,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {
	std::string	host_name;
	std::string	port;
	std::string	del = ":";
	size_t		del_pos;
	std::string	listen_value = tokens.at(block_start_idx + 1).val;


	(void) config;
	std::cout << "listen_value: " << listen_value << '\n';
	del_pos = listen_value.find(del);
	if (del_pos == std::string::npos) {
		/// ERROR, brr brr, error
		return (false);
	}
	std::cout << "del pos: " << del_pos << '\n';
	std::cout << "listen_value.size(): " << listen_value.size() << '\n';
	host_name = listen_value.substr(0, del_pos);
	port = listen_value.substr(del_pos + 1, listen_value.length() - del_pos - 2);
	std::cout << "host_name: " << host_name << '\n';
	std::cout << "port: " << port << '\n';
	return (true);
}

bool fillServerField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& block_start_idx,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {
	cfg_server_t	new_server;

	config.servers.push_back(new_server);
	config.is_correct = false;
	//for (size_t i = block_start_idx; i < tokens.size(); i++) {
	//	std::cout << "hello\n";
	//}
	return (true);
}

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
		std::cout << "Converting token No." << i << '\n';
		const t_config_token&	cur_token = tokens.at(i);
		if (cur_token.type == EVALUATED || cur_token.type == BRACE_OPEN) {
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
		//else if (cur_token.type == KEY) {
		//	parser = matchTokenValueToParserAccordingToContext(
		//		parsing_info, cur_token, context_stack);
		//	if (parser == NULL) {
		//		std::cout << CLR_RED << "SOMETHING SOMETHING ERROR\n" << CLR_NON;
		//		return (config);
		//	}
		//	else {
		//		(*parser)(config, parsing_info, i, tokens);
	  	//	}
		//}
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
