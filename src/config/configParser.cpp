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
		if (i >= parsers.size() || parsers.size() == 0) { // this should never happen
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
			std::cout << CLR_RED << "ERROR: TOKEN TYPE " << cur_token.val;
			std::cout << " NOT COVERED BY IF-ELSE\n" << CLR_NON;
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
	config.is_correct = true;
	return (config);
}

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

static bool checkConfigCompleteness(Config& config) {
	if (config.servers.size() == 0) {
		displayParserError("No server block found in config file", "Bad config");
		return (false);
	}
	for (size_t serv_i = 0; serv_i < config.servers.size(); serv_i++) {
		cfg_server_t&	cur_serv = config.servers.at(serv_i);
		if (cur_serv.ports.size() == 0) {
			cur_serv.ports.push_back(DEFAULT_PORT);
		}
		if (cur_serv.root.size() == 0) {
			displayParserError("Root directory not specified for server block", "Bad config");
			return (false);
		}
		if (cur_serv.client_max_body_size == -1) {
			displayParserError("Max body size not specified for server block", "Bad config");
			return (false);
		}
		// add default error pages here!
		if (cur_serv.locations.size() == 0) {
			displayParserError("No locations were specified for the server", "Bad config");
			return (false);
		}
		for (size_t loc_i = 0; loc_i < cur_serv.locations.size(); loc_i++) {
			t_location&	cur_loc = cur_serv.locations.at(loc_i);
			if (cur_loc.prefix.size() == 0) {
				displayParserError("Prefix not specified for location block", "Bad config");
				return (false);
			}
			// might not be necesary, check
			//if (cur_loc.root.size() == 0) {
			//	displayParserError("Root not specified for location block", "Bad config");
			//	return (false);
			//}
			if (cur_loc.index.size() == 0 && cur_loc.cgi_pass.path.size() == 0) {
				displayParserError("Index not specified for location block", "Bad config");
				return (false);
			}
		}
	}
	return (true);
}

std::optional<Config>	parseConfig(
	const char *const arg
) {
	std::string					config_path;
	std::vector<t_config_token>	tokens;
	const ParsingInfo			parsing_info;

	if (TEST_CONFIG == true) {
		config_path = CONFIG_PATH_TEST;
	}
	else {
		if (arg == NULL)
			config_path = CONFIG_PATH_DEFAULT; // do we need this?
		else
			config_path = arg;
	}
	std::ifstream	config_file(config_path);
	if (!config_file.is_open()) {
		displayParserError("Missing or inaccessible config file", "Config Error");
		return (std::nullopt);
	}

	tokens = tokenize(config_file);
	if (evaluateTokens(tokens) == 1)
		return (std::nullopt);
	else
		TEST_print_tokens(tokens);

	Config config = tokensToConfig(parsing_info, tokens);
	if (SHOW_CONFIG_PARSER_DEBUG == true) {
		std::cout << CLR_YEL << "DEBUG:\nFinished parsing config. ";
		std::cout << "Change SHOW_CONFIG_PARSER_DEBUG define to false to turn off parser debug messages\n";
		std::cout << CLR_NON << std::endl;
	}
	if (!config.is_correct) {
		displayParserError("Incorrect config. Can't start server", "Config Error");
		return (std::nullopt);
	}
	config.is_correct = checkConfigCompleteness(config);
	if (!config.is_correct) {
		displayParserError("Incorrect config. Can't start server", "Config Error");
		return (std::nullopt);
	}
	return (config);
}
