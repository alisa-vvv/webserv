/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 13:16:53 by avaliull          #+#    #+#             */
/*   Updated: 2026/07/17 13:27:50 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "AllowedCgi.hpp"
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <netinet/ip.h>

# ifndef CLR_NON
#  define	CLR_NON "\033[0m"
#  define	CLR_YEL "\033[93m"
#  define	CLR_CYA "\033[96m"
#  define	CLR_RED "\033[31m"
#  define	CLR_GRN "\033[92m"
#  define	CLR_MAG "\033[95m"
# endif // CLR_NON and other CLR defines

#define TEST_CONFIG false
#define CONFIG_PATH_DEFAULT "config/default.conf"
#define CONFIG_PATH_TEST "config/test.conf"
#define SHOW_CONFIG_PARSER_DEBUG false

#define DEFAULT_PORT 8080

// dummy class, defined in configParsingInfo.hpp
class ParsingInfo;

typedef enum e_config_mode {
	CONFIG_MODE_TEST,
	CONFIG_MODE_DEFAULT,
	CONFIG_MODE_PASS,
}	e_config_mode;

typedef enum e_token_type {
	UNDEFINED_TYPE,
	BLOCK_NAME,
	BLOCK_PREFIX,
	BRACE_OPEN,
	BRACE_CLOSE,
	KEY,
	VALUE,
	SEMICOLON,
	NEWLINE,
	EVALUATED,
} e_token_type;

//
typedef struct	t_config_token {
	e_token_type	type;
	std::string		val;
	int				line_number;
}	t_config_token;

typedef enum	e_state_label {
	FINDING_BLOCK,
	FINDING_VALUES,
	FOUND_ERROR,
}	e_state_label;

/*
* Data structures that are used inside Config class
*/

	/*	methods	*/
enum httpMethod {
	GET,
	POST,
	DELETE,
	UNKNOWN,
};

	/* cgi_pass	*/
typedef struct t_cgi_pass {
	std::string		path;
	e_cgi_extension	extension;
}	t_cgi_pass;
/**/

	/*	return	*/
typedef struct t_return {
	int			code = 0;
	std::string	target;
}	t_return;

	/*	location	*/
typedef struct t_location {
	std::string					prefix; // needed
	std::string					root; // needed
	std::string					index; // opt if cgi_pass is not set
	bool						autoindex = false; // needed
	std::map<httpMethod, bool>	allowed_methods { {GET, false}, {POST, false}, {DELETE, false} }; // needed
	std::string					upload_store; // opt
	t_cgi_pass					cgi_pass; // opt
	int							client_max_body_size = -1; // opt
	t_return					returns;
}	t_location;

	/*	server	*/
typedef struct cfg_server_t {
	std::vector<std::string>	server_names;
	uint32_t					ip_addr = INADDR_ANY;
	std::vector<int>			ports;
	std::string					root;
	int							client_max_body_size = -1;
	std::map<int, std::string>	error_pages;
	std::vector<t_location>		locations;
	t_cgi_pass					cgi_pass; // opt
	bool						autoindex = false;
}	cfg_server_t;
/*
*/

class	Config {
public:
	bool	is_correct = false;
	std::vector<cfg_server_t>	servers;
};

std::optional<Config>		parseConfig(const char *const arg);
std::vector<t_config_token>	tokenize(std::ifstream&	config_file);
int							evaluateTokens(std::vector<t_config_token>& tokens);

/*
* Error
*/
void	displayParserError(
	const std::string& message,
	const std::optional<std::string>& prefix
);
void	configParserError(
	Config& config,
	const std::string& message,
	const std::optional<std::string>& prefix,
	const std::optional<int> line_number
);
void	locationValidationError(
	const std::string& message,
	const size_t server_index,
	const size_t location_index
);

/*
* Helpers
*/
bool				tokenIsAlpha(t_config_token& token);
bool				pathIsValid(std::string path);
constexpr size_t	lengthOfInt(int val) {
	int	length = 0;

	do {
		length++;
		val /= 10;
	} while(val != 0);
	return (length);
}
