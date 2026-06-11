/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   serverBlockParsers.cpp                                  :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/11 12:11:35 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/11 12:52:06 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include "configParsingInfo.hpp"
#include "configParserTEST.hpp"
#include <iostream>
#include <arpa/inet.h>

bool fillServerField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {
	cfg_server_t	new_server;

	config.servers.push_back(new_server);
	config.is_correct = false;
	tokens.at(token_index).type = EVALUATED;
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

	del_pos = listen_value.find(del);
	if (del_pos == std::string::npos) {
		/// ERROR, brr brr, error
		return (false);
	}
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
				return (false);
			}
			address_part = host_name.substr(start, end);
			try {
				byte_val = stoi(address_part);
			} catch (std::exception invalid_argument) {
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
		byte_val = stoi(host_name);
		if (byte_val > 255 || byte_val < 0) {
			/// ERROR, brr brr, error
			return (false);
		}
		result += byte_val;
		config.servers.back().listen.back().ip_addr = htonl(result);
	}

	//	test
	printParserDebug("server listen field",
				  "config.servers.back().listen.back().ip_addr", true,
				  std::nullopt, std::nullopt, config.servers.back().listen.back().ip_addr);
	printParserDebug("server listen field",
				  "config.servers.back().listen.back().port", false,
				  std::nullopt, config.servers.back().listen.back().port, std::nullopt);
	if (SHOW_CONFIG_PARSER_DEBUG == true) {
		char buffer[32] { 0 };
		std::cout << "checking ip as str: ";
		std::cout << inet_ntop(AF_INET, &config.servers.back().listen.back().ip_addr, buffer, 32) << '\n';
	}
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
	printParserDebug("server root field", "config.servers.back().root", true,
				  config.servers.back().root, std::nullopt, std::nullopt);
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

	printParserDebug("server location block",
				  "config.servers.back().locations.back().prefix", true,
				  config.servers.back().locations.back().prefix,
				  std::nullopt, std::nullopt);

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

	printParserDebug(
					"server error page field",
					"error_code",
					true,
					std::nullopt,
					error_code,
					std::nullopt
	);
	printParserDebug(
					"server error page field",
					"config.servers.back().error_pages[error_code]",
					false,
					config.servers.back().error_pages[error_code],
					std::nullopt,
					std::nullopt
	);

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

	printParserDebug(
					"server client_max_body_size field",
					"config.servers.back().client_max_body_size",
					true,
					std::nullopt,
					std::nullopt,
					config.servers.back().client_max_body_size
	);

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

	printParserDebug(
					"server cgi_pass field",
					"config.servers.back().cgi_pass.path",
					true,
					config.servers.back().cgi_pass.path,
					std::nullopt,
					std::nullopt
	);
	printParserDebug(
					"server cgi_pass field",
					"config.servers.back().cgi_pass.extension",
					false,
					std::nullopt,
					config.servers.back().cgi_pass.extension,
					std::nullopt
	);

	return (true);
}

bool fillServerAutoindex(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
) {

	if (tokens.at(token_index + 1).val == "on") {
		config.servers.back().autoindex = true;
	}
	else if (tokens.at(token_index + 1).val == "off") {
		config.servers.back().autoindex = false;
	}
	else {
		configParserError(
			config,
			"autoindex value can only be on/off",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;

	printParserDebug(
					"server autoindex field",
					"config.servers.back().autoindex",
					true,
					std::nullopt,
					config.servers.back().autoindex,
					std::nullopt
	);

	return (true);
}
