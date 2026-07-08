/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   serverBlockParsers.cpp                                  :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/11 12:11:35 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/12 19:02:04 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include "configParsingInfo.hpp"
#include "configParserTEST.hpp"
#include <iostream>
#include <arpa/inet.h>

bool	fillServerField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	cfg_server_t	new_server;

	config.servers.push_back(new_server);
	config.is_correct = false;
	tokens.at(token_index).type = EVALUATED;
	return (true);
}

static bool	fillRootField(
	e_context context,
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (!pathIsValid(tokens.at(token_index + 1).val)) {
		configParserError(
			config,
			"value of field root is not a valid path",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
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

static bool	fillCgiPass(
	e_context context,
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
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
	if (ext_token.val == CGI_EXT_STR_PY) {
		*extension_store = CGI_EXT_PY;
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

static bool	fillAutoIndex(
	e_context context,
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
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

bool	fillListenField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	std::string	host_name;
	std::string	port;
	std::string	del = ":";
	size_t		del_pos;
	std::string	listen_value = tokens.at(token_index + 1).val;

	del_pos = listen_value.find(del);
	if (del_pos == std::string::npos) {
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
		/// ERROR, brr brr, error
		return (false);
	}
	try {
		config.servers.back().ports.push_back(stoi(port));
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

bool	fillServerRootField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (config.servers.back().root.size() != 0) {
		configParserError(
			config,
			"duplicate root fields",
			"Config Error",
			tokens.at(token_index).line_number);
		return (false);
	}
	return (fillRootField(SERVER, config, token_index, tokens));
}

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

bool	fillServerErrorPageField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
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

bool	fillServerMaxBodySize(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
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
	if (config.servers.back().client_max_body_size > CLIENT_MAX_BODY_SIZE) {
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

static std::optional<t_return>	fillReturn(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
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

bool	fillServerReturn(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	std::optional<t_return>	new_return = fillReturn(config, token_index, tokens);
	if (new_return == std::nullopt) {
		return (false);
	}
	config.servers.back().returns = *new_return;

	printParserDebug(
		"server return code field",
		"config.servers.back().returns.code",
		true,
		std::nullopt,
		std::nullopt,
		config.servers.back().returns.code
	);
	printParserDebug(
		"server return target field",
		"config.servers.back().returns.target",
		true,
		config.servers.back().returns.target,
		std::nullopt,
		std::nullopt
	);

	return (true);
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

bool	fillLocationIndexField( // TEST THIS
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
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
					"config.servers.back().autoindex",
					true,
					config.servers.back().locations.back().index,
					std::nullopt,
					std::nullopt
	);

	return (true);
}

bool	fillLocationAllowedMethodsField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	int			i = token_index + 1;
	e_method	method;

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
		if (config.servers.back().locations.back().allowed_methods[method] == true) { // is this even necessary?
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

bool	fillLocationUploadStoreField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
) {
	if (tokens.at(token_index + 2).type == VALUE) {
		configParserError(
						config,
						"upload_store can only have one directory",
						"Config Error",
						tokens.at(token_index).line_number
		);
		return (false);
	}
	if (!pathIsValid(tokens.at(token_index +1).val)) {
		configParserError(
						config,
						"upload_store value is not a valid path",
						"Config Error",
						tokens.at(token_index).line_number
		);
		return (false);
	}
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
