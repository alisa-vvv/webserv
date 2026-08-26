/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   serverBlockParsers.cpp                                  :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/11 12:11:35 by avaliull            #+#    #+#           */
/*   Updated: 2026/08/26 13:54:24 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include "configParsingInfo.hpp"
#include "configParserTEST.hpp"
#include <iostream>
#include <algorithm>
#include <arpa/inet.h>

static bool	argumentNotValidPath(
	Config& config,
	const std::vector<t_config_token>& tokens,
	const size_t token_index,
	const size_t arg_index
) {
	if (!pathIsValid(tokens.at(arg_index).val)) {
		const std::string	error_msg =
			"value of field " + tokens.at(token_index).val + " " +
			"is not a valid path";
		configParserError(
			config,
			error_msg,
			"Config Error",
			tokens.at(arg_index).line_number);
		return (true);
	}
	return (false);
}

static bool	isAboveMaxArgs(
	Config& config,
	const int max_args,
	const std::vector<t_config_token>& tokens,
	const size_t token_index
) {
	if (tokens.at(token_index + 1 + max_args).type == VALUE) {
		const std::string	error_msg =
			"field " + tokens.at(token_index).val + " can only have " +
			std::to_string(max_args) + ' ' +
			((max_args > 1) ? "arguments":"argument");

		configParserError(
			config,
			error_msg,
			"Config Error",
			tokens.at(token_index).line_number);
		return (true);
	}
	return (false);
}

bool	fillServerField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	cfg_server_t	new_server;

	config.servers.push_back(new_server);
	tokens.at(token_index).type = EVALUATED;
	return (true);
}

// 1. has to be a path - DONE
// 2. only one arg - DONE
static bool	fillRootField(
	e_context context,
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (isAboveMaxArgs(config, 1, tokens, token_index))
		return (false);
	if (argumentNotValidPath(config, tokens, token_index, token_index + 1))
		return (false);

	if (context == LOCATION) {
		config.servers.back().locations.back().root = tokens.at(token_index + 1).val;
		printParserDebug(
			"location root field",
			"config.servers.back().locations.back().root",
			true,
			config.servers.back().locations.back().root,
			std::nullopt,
			std::nullopt
		);
	}
	else if (context == SERVER) {
		config.servers.back().root = tokens.at(token_index + 1).val;
		printParserDebug(
			"server root field",
			"config.servers.back().root",
			true,
			config.servers.back().root,
			std::nullopt,
			std::nullopt
		);
	}
	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;
	return (true);
}

// 1. first argument has to be an allowed extension - DONE
// 2. second argument has to be a valid path - DONE
// 3. maximum two arguments - DONE
static bool	fillCgiPass(
	e_context context,
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	static const AllowedCgi	allowed_cgi;

	if (isAboveMaxArgs(config, 2, tokens, token_index))
		return (false);

	t_config_token	ext_token = tokens.at(token_index + 1);
	t_config_token	path_token = tokens.at(token_index + 2);


	e_cgi_extension*	extension_store;
	std::string*		path_store;

	if (context == SERVER) {
		extension_store = &config.servers.back().cgi_pass.extension;
		path_store = &config.servers.back().cgi_pass.path;
	}
	else if (context == LOCATION) {
		extension_store = &config.servers.back().locations.back().cgi_pass.extension;
		path_store = &config.servers.back().locations.back().cgi_pass.path;
	}

	if (path_token.type != VALUE) {
		configParserError(
			config,
			"missing path to CGI script in cgi_pass",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}

	*extension_store = allowed_cgi.match(ext_token.val);
	if (*extension_store == CGI_EXT_UNKNOWN) {
		configParserError(
			config,
			"CGI extension in cgi_pass doesn't match allowed extensions",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}

	if (argumentNotValidPath(config, tokens, token_index, token_index + 2))
		return (false);
	if (path_token.val.size() < ext_token.val.size() + 2) {
		configParserError(
			config,
			"path to CGI file in cgi_pass is not a valid file or extension",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	for (size_t i = path_token.val.size() - ext_token.val.size(), j = 0;
			i < path_token.val.size(); i++) {
		if (path_token.val.at(i) != ext_token.val.at(j)) {
			std::cout << "path_token.val.at(i): " << path_token.val.at(i);
			std::cout << ", ext_token.val.at(j): " << ext_token.val.at(j) << '\n';
			configParserError(
				config,
				"path to CGI file in cgi_pass doesn't match given extension",
				"Config Error",
				tokens.at(token_index).line_number);
			return (false);
		}
		j++;
	}
	*path_store = path_token.val;

	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;
	tokens.at(token_index + 2).type = EVALUATED;

	if (context == SERVER) {
		printParserDebug(
						"server cgi_pass field",
						"config.servers.back().cgi_pass.path",
						true,
						*path_store,
						std::nullopt,
						std::nullopt
		);
		printParserDebug(
						"server cgi_pass field",
						"config.servers.back().cgi_pass.extension",
						false,
						std::nullopt,
						*extension_store,
						std::nullopt
		);
	}
	else if (context == LOCATION) {
		printParserDebug(
						"location cgi_pass field",
						"config.servers.back().location.back().cgi_pass.path",
						true,
						*path_store,
						std::nullopt,
						std::nullopt
		);
		printParserDebug(
						"location cgi_pass field",
						"config.servers.back().location.back().cgi_pass.extension",
						false,
						std::nullopt,
						*extension_store,
						std::nullopt
		);
	}

	return (true);
}

// 1. can only be "on" or "off" - DONE.
// 2. can only have 1 argument - DONE.
static bool	fillAutoIndex(
	e_context context,
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (isAboveMaxArgs(config, 1, tokens, token_index))
		return (false);

	bool*	store;
	if (context == SERVER) {
		store = &config.servers.back().autoindex;
	}
	else if (context == LOCATION) {
		store = &config.servers.back().locations.back().autoindex;
	}
	if (tokens.at(token_index + 1).val == "on") {
		*store = true;
	}
	else if (tokens.at(token_index + 1).val == "off") {
		*store = false;
	}
	else {
		configParserError(
						config,
						"autoindex value can only be on/off",
						"Config Error",
						tokens.at(token_index).line_number
		);
		return (false);
	}
	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;

	if (context == SERVER) {
		printParserDebug(
						"server autoindex field",
						"config.servers.back().autoindex",
						true,
						std::nullopt,
						*store,
						std::nullopt
		);
	}
	else if (context == LOCATION) {
		printParserDebug(
						"location autoindex field",
						"config.servers.back().locations.back().autoindex",
						true,
						std::nullopt,
						*store,
						std::nullopt
		);
	}

	return (true);
}

// no special conditions
bool	fillServerNameField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {

	int	i = token_index + 1;
	while (tokens.at(i).type == VALUE) {
		config.servers.back().server_names.push_back(tokens.at(i).val);
		tokens.at(i).type = EVALUATED;
		i++;
	}

	tokens.at(token_index).type = EVALUATED;

	for (size_t j = 0; j < config.servers.back().server_names.size(); j++) {
		printParserDebug(
			"server_name field",
			"config.servers.back().server_names.at(i)",
			true,
			config.servers.back().server_names.at(j),
			std::nullopt,
			std::nullopt
		);
	}

	return (true);
}

// todo this.
// can be:
// 1. just the port - like 8080
// 2. host name and port - like 192.x.x.x:8080
//
// 1. port has to be numeric - DONE.
// 2. ip has to have correct format - DONE.
// 3. no duplicate ports in a single virtual server - DONE.
// 4. can only have one argument

bool	fillListenField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (isAboveMaxArgs(config, 1, tokens, token_index))
		return (false);

	std::string	host_name;
	std::string	port;
	std::string	del = ":";
	size_t		del_pos;
	std::string	listen_value = tokens.at(token_index + 1).val;

	del_pos = listen_value.find(del);
	if (del_pos != std::string::npos) {
		if (config.servers.back().ip_addr != INADDR_ANY) {
			configParserError(
				config,
				"virtual server can listen on only one ip address",
				"Config Error",
				tokens.at(token_index).line_number
			);
			return (false);
		}
		// if there is no colon, only port is specified
		// if host was previosuly specified for this server, should use that
		// if not, should use IPADDR_ANY
	}
	host_name = listen_value.substr(0, del_pos);
	port = listen_value.substr(del_pos + 1, listen_value.length() - del_pos - 1);
	if (port.find_first_not_of("0123456789") != std::string::npos) {
		configParserError(
			config,
			"port value has to be a positive integer",
			"Config Error",
			tokens.at(token_index).line_number
		);
		return (false);
	}

	int	port_val;
	try {
		port_val = stoi(port);
	} catch (...) {
		configParserError(
			config,
			"port value outside of integer range",
			"Config Error",
			tokens.at(token_index).line_number
		);
		return (false);
	}
	if (std::find(config.servers.back().ports.begin(),
			   config.servers.back().ports.end(), port_val)
			!= config.servers.back().ports.end()) {
		configParserError(
			config,
			"duplicate ports not allowed inside a single virtual server",
			"Config Error",
			tokens.at(token_index).line_number
		);
		return (false);
	}
	config.servers.back().ports.push_back(port_val);
	// Parse IP adress and convert it into uint32_t form
	if (del_pos != std::string::npos)
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
				configParserError(
					config,
					"incorrect format for ip address",
					"Config Error",
					tokens.at(token_index).line_number
				);
				return (false);
			}
			dot_count++;
			if (end - start > 3) {
				configParserError(
					config,
					"incorrect format for host address",
					"Config Error",
					tokens.at(token_index).line_number
				);
				return (false);
			}
			address_part = host_name.substr(start, end);
			if (address_part.find_first_not_of("0123456789") != std::string::npos) {
				configParserError(
					config,
					"incorrect format for host address",
					"Config Error",
					tokens.at(token_index).line_number
				);
				return (false);
			}
			try {
				byte_val = stoi(address_part);
			} catch (std::exception invalid_argument) {
				configParserError(
					config,
					"incorrect format for host address",
					"Config Error",
					tokens.at(token_index).line_number
				);
				return (false);
			}
			if (byte_val > 255 || byte_val < 0) {
				configParserError(
					config,
					"incorrect format for host address",
					"Config Error",
					tokens.at(token_index).line_number
				);
				return (false);
			}
			result += byte_val << bit_shift_val;
			bit_shift_val -= 8;
			host_name.erase(0, end + 1);
		}
		byte_val = stoi(host_name);
		if (byte_val > 255 || byte_val < 0) {
			configParserError(
				config,
				"incorrect format for host address",
				"Config Error",
				tokens.at(token_index).line_number
			);
			return (false);
		}
		result += byte_val;
		host_name.erase(0, end);
		if (!host_name.empty()) {
			configParserError(
				config,
				"incorrect format for host address",
				"Config Error",
				tokens.at(token_index).line_number
			);
			return (false);
		}
		config.servers.back().ip_addr = htonl(result);
	}

	//	test
	printParserDebug("server listen field",
				  "config.servers.back().listen.back().ip_addr", true,
				  std::nullopt, std::nullopt, config.servers.back().ip_addr);
	printParserDebug("server listen field",
				  "config.servers.back().listen.back().port", false,
				  std::nullopt, config.servers.back().ports.back(), std::nullopt);
	if (SHOW_CONFIG_PARSER_DEBUG == true) {
		char buffer[32] { 0 };
		std::cout << "checking ip as str: ";
		std::cout << inet_ntop(AF_INET, &config.servers.back().ip_addr, buffer, 32) << '\n';
	}
	//	testend

	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;
	return (true);
}

// has to be a valid path - DONE.
// can ony be one arg - DONE.

bool	fillServerRootField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	return (fillRootField(SERVER, config, token_index, tokens));
}

// needs a prefix - DONE.
// prefix needs to be a path - DONE.
bool	fillServerLocationField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
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

// 1. can have a maximum of two arguments - DONE.
// 2. needs to have a code and an html page - DONE
// 3. code needs to be within error range - DONE.
bool	fillServerErrorPageField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (isAboveMaxArgs(config, 2, tokens, token_index))
		return (false);

	int				error_code;
	t_config_token&	error_code_token = tokens.at(token_index + 1);
	t_config_token&	error_page_token = tokens.at(token_index + 2);

	if (error_code_token.type != VALUE
		&& error_page_token.type != VALUE) {
		configParserError(
			config,
			"HTML error field needs two values: error code and error page path",
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
	if (argumentNotValidPath(config, tokens, token_index, token_index + 2))
		return (false);
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

std::string	fillMaxBodySize(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (isAboveMaxArgs(config, 1, tokens, token_index))
		return ("");

	t_config_token&	size_token = tokens.at(token_index + 1);

	if (size_token.val.length() > lengthOfInt(CLIENT_MAX_BODY_SIZE)) {
		configParserError(
			config,
			"value for client_max_body_size option is too big",
			"Config Error",
			tokens.at(token_index).line_number);
		return ("");
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
		return ("");
	}
	return (size_token.val);
}

// 1. can only be one arguent - DONE.
// 2. has to be a number or a number + M or m - DONE.
// 3. number has to be lower than defined maximum - DONE.
bool	fillServerMaxBodySize(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	const std::string	res = fillMaxBodySize(config, token_index, tokens);
	if (res.size() == 0)
		return (false);

	config.servers.back().client_max_body_size = std::stol(res);
	if (config.servers.back().client_max_body_size > CLIENT_MAX_BODY_SIZE) {
		configParserError(
			config,
			"value for client_max_body_size too big",
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

bool	fillServerCgiPass(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	return (fillCgiPass(SERVER, config, token_index, tokens));
}

bool	fillServerAutoIndex(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	return (fillAutoIndex(SERVER, config, token_index, tokens));
}

// 1. can have a maximum of two (three if we do strings) max. arguments - DONE.
// 2. first argument is either error code or a URI - DONE.
// 3. error code must be within defined range - DONE.
// 4. URI must be a path or an http link - DONE.
static std::optional<t_return>	fillReturn(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (isAboveMaxArgs(config, 2, tokens, token_index))
		return (std::nullopt);
	t_return		new_return;
	t_config_token& first_arg = tokens.at(token_index + 1);
	t_config_token* redirect_target_tok = &tokens.at(token_index + 1);

	if (first_arg.val.length() == 3) {
		if (first_arg.val.find_first_not_of("0123456789") != std::string::npos) {
			return (std::nullopt);
		}
		const int	code_val = std::stoi(first_arg.val);
		if (code_val < RETURN_CODE_LOWEST || code_val > RETURN_CODE_HIGHEST) {
			configParserError(
				config,
				"return code outside of allowed range",
				"Config Error",
				tokens.at(token_index).line_number);
			return (std::nullopt);
		}
		new_return.code = code_val;
		if (tokens.at(token_index + 2).type == VALUE)
			redirect_target_tok = &tokens.at(token_index + 2);
	}
	if (&first_arg != redirect_target_tok) {
		if (redirect_target_tok->val.find("http://") != 0
				&& redirect_target_tok->val.find("https://") != 0 &&
				redirect_target_tok->val.at(0) != '/') {
			configParserError(
				config,
				"return target must start with http:// or https:// if URL or with / if path",
				"Config Error",
				tokens.at(token_index).line_number);
			return (std::nullopt);
		}
		new_return.target = redirect_target_tok->val;
	}

	tokens.at(token_index).type = EVALUATED;
	first_arg.type = EVALUATED;
	redirect_target_tok->type = EVALUATED;

	return (new_return);
}

bool	fillLocationReturn(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	std::optional<t_return>	new_return = fillReturn(config, token_index, tokens);
	if (new_return == std::nullopt) {
		return (false);
	}
	config.servers.back().locations.back().returns = *new_return;

	printParserDebug(
		"location return code field",
		"config.servers.back().locations.back().returns.code",
		true,
		std::nullopt,
		std::nullopt,
		config.servers.back().locations.back().returns.code
	);
	printParserDebug(
		"location return target field",
		"config.servers.back().locations.back().returns.target",
		true,
		config.servers.back().locations.back().returns.target,
		std::nullopt,
		std::nullopt
	);

	return (true);
}

bool	fillLocationRootField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	return (fillRootField(LOCATION, config, token_index, tokens));
}

// 1. has to end with .html - DONE.
// 2. 1 argument max - DONE.
// 3. can be called something that's not index.html?
bool	fillLocationIndexField( // TEST THIS
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (isAboveMaxArgs(config, 1, tokens, token_index)) {
		return (false);
	}

	std::string	val_str = tokens.at(token_index + 1).val;

	if (val_str.size() < 6) {
		configParserError(
						config,
						"index has to be a named .html file",
						"Config Error",
						tokens.at(token_index).line_number
		);
		return (false);
	}
	if (val_str.substr(val_str.size() - 5, 5) != ".html") {
		configParserError(
						config,
						"index has to be a .html file",
						"Config Error",
						tokens.at(token_index).line_number
		);
		return (false);
	}
	config.servers.back().locations.back().index = tokens.at(token_index + 1).val;

	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;

	printParserDebug(
					"location index field",
					"config.servers.back().locations.back().index",
					true,
					config.servers.back().locations.back().index,
					std::nullopt,
					std::nullopt
	);

	return (true);
}

// 1. can be any amount of args - DONE.
// 2. args have to match pre-defined methods - DONE.
// 3. no duplicates - DONE.
bool	fillLocationAllowedMethodsField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	int			i = token_index + 1;
	httpMethod	method;

	while (tokens.at(i).type == VALUE) {
		if (tokens.at(i).val == "GET")
			method = GET;
		else if (tokens.at(i).val == "POST")
		 	method = POST;
		else if (tokens.at(i).val == "DELETE")
			method = DELETE;
		else {
			configParserError(
							config,
							"unknown method in allowed_methods field",
							"Config Error",
							tokens.at(token_index).line_number
			);
			return (false);
		}
		if (config.servers.back().locations.back().allowed_methods[method] == true) {
			configParserError(
				config,
				"duplicate method in allowed_methods field",
				"Config Error",
				tokens.at(token_index).line_number
			);
			return (false);
		}
		config.servers.back().locations.back().allowed_methods[method] = true;

		printParserDebug(
			"location allowed methods field",
			"config.servers.back().allowed_methods[" + tokens.at(i).val + "]",
			true,
			std::nullopt,
			config.servers.back().locations.back().allowed_methods[method],
			std::nullopt
		);

		i++;
	}

	for (int j = token_index; j < i; j++) {
		tokens.at(j).type = EVALUATED;
	}

	return (true);
}

// 1. has to be a valid path - DONE.
// 2. max 1 element - DONE.
bool	fillLocationUploadStoreField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (isAboveMaxArgs(config, 1, tokens, token_index))
		return (false);
	if (argumentNotValidPath(config, tokens, token_index, token_index + 1))
		return (false);

	config.servers.back().locations.back().upload_store = tokens.at(token_index + 1).val;

	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;

	printParserDebug(
					"location upload store field",
					"config.servers.back().locations.back().upload_store",
					true,
					config.servers.back().locations.back().upload_store,
					std::nullopt,
					std::nullopt
	);

	return (true);
}

bool	fillLocationCgiPass(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	return (fillCgiPass(LOCATION, config, token_index, tokens));
}

bool	fillLocationAutoIndex(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	return (fillAutoIndex(LOCATION, config, token_index, tokens));
}

bool	fillLocationMaxBodySize(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	const std::string	res = fillMaxBodySize(config, token_index, tokens);
	if (res.size() == 0)
		return (false);

	config.servers.back().locations.back().client_max_body_size = std::stol(res);
	if (config.servers.back().locations.back().client_max_body_size > CLIENT_MAX_BODY_SIZE) {
		configParserError(
			config,
			"value for client_max_body_size too big",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	tokens.at(token_index).type = EVALUATED;
	tokens.at(token_index + 1).type = EVALUATED;

	printParserDebug(
		"location client_max_body_size field",
		"config.servers.back().locations.back().client_max_body_size",
		true,
		std::nullopt,
		std::nullopt,
		config.servers.back().locations.back().client_max_body_size
	);

	return (true);
}

