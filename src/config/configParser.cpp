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
#include <optional>
#include <arpa/inet.h> // for testing only, remove!

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

/*
* Helpers
*/
bool	tokenIsAlpha(t_config_token& token) {
	if (token.val.find_first_not_of("abcdefghijklmnopqrstuvwxyz_") != std::string::npos)
		return (false);
	return (true);
}

constexpr int	lengthOfInt(int val) {
	int	length = 0;

	do {
		length++;
		val /= 10;
	} while(val != 0);
	return (length);
}

bool	pathIsValid(std::string path) { // add more checks here
	if (path.at(0) != '/')
		return (false);
	return (true);
}
/*
*/

bool fillServerField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {
	cfg_server_t	new_server;

	config.servers.push_back(new_server);
	config.is_correct = false;
	//for (size_t i = token_index; i < tokens.size(); i++) {
	//	std::cout << "hello\n";
	//}
	return (true);
}

bool fillListenField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {
	std::string	host_name;
	std::string	port;
	std::string	del = ":";
	size_t		del_pos;
	std::string	listen_value = tokens.at(token_index + 1).val;

	//	test
	std::cout << "listen_value: " << listen_value << '\n';
	//	testend

	del_pos = listen_value.find(del);
	if (del_pos == std::string::npos) {
		/// ERROR, brr brr, error
		return (false);
	}

	//	test
	std::cout << "del pos: " << del_pos << '\n';
	std::cout << "listen_value.size(): " << listen_value.size() << '\n';
	//	testend

	host_name = listen_value.substr(0, del_pos);
	port = listen_value.substr(del_pos + 1, listen_value.length() - del_pos - 1);
	if (port.find_first_not_of("0123456789") != std::string::npos) {
		/// ERROR, brr brr, error
		return (false);
	}

	config.servers.back().listen.push_back(listen_t {});
	try {
		config.servers.back().listen.back().port = stoi(port);
	} catch(std::exception out_of_range) {
		/// ERROR, brr brr, error
		std::cout << "exception!\n";
		return (false);
	}

	//	test
	std::cout << "host_name: " << host_name << '\n';
	std::cout << "port: " << port << '\n';
	//	testend


	// Parse IP adress and convert it into uint32_t form
	{
		std::string	address_part;
		int			byte_val = 0;
		size_t		start = 0;
		size_t		end = 0;
		int			dot_count = 0;
		int			bit_shift_val = 24;
		uint32_t	result = 0;

		while (dot_count < 3) {
			end = host_name.find_first_of(".");
			if (end == std::string::npos) {
				return (false);
			}
	   		dot_count++;
			if (end - start > 3) {
				/// ERROR, brr brr, error
				std::cout << "ERROR, brr brr, error\n";
				std::cout << "end: " << end << '\n';
				std::cout << "start: " << start << '\n';
				std::cout << "dot_count: " << dot_count << '\n';
				return (false);
			}
			address_part = host_name.substr(start, end);
			try {
				byte_val = stoi(address_part);
			} catch (std::exception invalid_argument) {
				/// ERROR, brr brr, error
				//std::cout << "exception!\n";
				configParserError(
					config,
					"Host address bad",
					"Config Error",
					tokens.at(token_index).line_number
				);
				return (false);
			}
			if (byte_val > 255 || byte_val < 0) {
				/// ERROR, brr brr, error
				return (false);
			}
			result += byte_val << bit_shift_val;
			bit_shift_val -= 8;
			host_name.erase(0, end + 1);
		}
		byte_val = stoi(address_part);
		if (byte_val > 255 || byte_val < 0) {
			/// ERROR, brr brr, error
			return (false);
		}
		result += byte_val << bit_shift_val;
		bit_shift_val -= 8;
		config.servers.back().listen.back().ip_addr = htonl(result);
	}
	//	test
	char buffer[32] { 0 };
	std::cout << "checking unint32 op value: ";
	std::cout << config.servers.back().listen.back().ip_addr;
	std::cout << "\nchecking ip as str: ";
	std::cout << inet_ntop(AF_INET, &config.servers.back().listen.back().ip_addr, buffer, 32) << '\n';
	//	testend

	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;
	return (true);
}

bool fillServerRootField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {
	if (!pathIsValid(tokens.at(token_index + 1).val)) {
		configParserError(
			config,
			"value of field root is not a valid path",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	config.servers.back().root = tokens.at(token_index + 1).val;
	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;
	return (true);
}

bool fillServerLocationField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {
	config.servers.back().locations.push_back(t_location {});
	if (tokens.at(token_index + 1).type != BLOCK_PREFIX) {
		configParserError(
			config,
			"location block need a prefix path before braces",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	if (!pathIsValid(tokens.at(token_index + 1).val)) {
		configParserError(
			config,
			"value of a prefix must start with a forward slash to be a valid path",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	config.servers.back().locations.back().prefix = tokens.at(token_index + 1).val;
	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;
	return (true);
}

bool fillServerErrorPageField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {
	int				error_code;
	t_config_token&	error_code_token = tokens.at(token_index + 1);
	t_config_token&	error_page_token = tokens.at(token_index + 2);

	if (error_code_token.type != VALUE
		&& error_page_token.type != VALUE) {
		configParserError(
			config,
			"HTML error field needs to keys: error code and error page path",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	if (error_code_token.val.size() != 3) {
		configParserError(
			config,
			"html error code not in defined range (4XX and 5XX)",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}	
	try {
		error_code = std::stoi(error_code_token.val);
	} catch (std::exception invalid_argument) {
		configParserError(
			config,
			"html error code not in defined range (4XX and 5XX)",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	if (error_code > 599 || error_code < 400) {
		configParserError(
			config,
			"html error code not in defined range (4XX and 5XX)",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	if (!pathIsValid(error_page_token.val)) {
		configParserError(
			config,
			"error page path must begin with a forward slash",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	auto it = config.servers.back().error_pages.find(error_code);
	if (it != config.servers.back().error_pages.end()) {
		config.servers.back().error_pages[error_code] = error_page_token.val;
	}
	else
		config.servers.back().error_pages.insert({error_code, error_page_token.val});
	tokens.at(token_index).type = EVALUATED;
	error_code_token.type = EVALUATED;
	error_page_token.type = EVALUATED;
	return (true);
}

bool fillServerMaxBodySize(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {
	t_config_token&	size_token = tokens.at(token_index + 1);

	// add check for if it's bigger than max value
	if (size_token.val.length() > lengthOfInt(CLIENT_MAX_BODY_SIZE)) {
		configParserError(
			config,
			"value for client_max_body_size option is too long",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	if (size_token.val.back() == 'm' || size_token.val.back() == 'M') {
		size_token.val.pop_back();
	}
	if (size_token.val.find_first_not_of("0123456789") != std::string::npos) {
		configParserError(
			config,
			"format for client_max_body_size has to be [positive int][opt. M or m]",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	config.servers.back().client_max_body_size = std::stol(size_token.val);
	if (config.servers.back().client_max_body_size > CLIENT_MAX_BODY_SIZE) { // ADD ACTUAL VALUE
		configParserError(
			config,
			"value for client_max_body_size to big",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;
	return (true);
}

bool fillServerCgiPass(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {
	t_config_token	ext_token = tokens.at(token_index + 1);
	t_config_token	path_token = tokens.at(token_index + 2);

	if (path_token.type != VALUE) {
		configParserError(
			config,
			"missing path to CGI script in cgi_pass",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	if (ext_token.val == CGI_EXT_STR_PY) {
		config.servers.back().cgi_pass.extension = CGI_EXT_PY;
	}
	else {
		configParserError(
			config,
			"CGI extension in cgi_pass doesn't match allowed extensions",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	if (!pathIsValid(path_token.val)) {
		configParserError(
			config,
			"invalid path to CGI in cgi_pass",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	config.servers.back().cgi_pass.path = path_token.val;
	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;
	tokens.at(token_index + 2).type = EVALUATED;
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
