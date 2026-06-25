/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 13:16:53 by avaliull          #+#    #+#             */
/*   Updated: 2026/06/25 15:09:59 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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

#define CONFIG_PATH_DEFAULT "config/default.conf"
#define CONFIG_PATH_TEST "config/test.conf"
#define SHOW_CONFIG_PARSER_DEBUG true

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


typedef struct	t_config_token {
	e_token_type	type;
	std::string		val;
	int				line_number;
}	t_config_token;

typedef enum	e_state_label {
	FINDING_BLOCK,
	FINDING_VALUES,
}	e_state_label;

/*
* Data structures that are used inside Config class
*/
	/*	error_page	*/
typedef struct t_error_page {
	int			error_num;
	std::string	redirect;
}	t_error_page;

	/*	methods	*/
typedef enum {
	GET,
	POST,
	DELETE,
}	e_method;

	/* cgi_pass	*/
#define CGI_EXT_STR_PY ".py"

typedef enum	e_cgi_extension {
	CGI_EXT_PY,
}	e_cgi_extension;

typedef struct t_cgi_pass {
	std::string		path;
	e_cgi_extension	extension;
}	t_cgi_pass;
/**/

	/*	return	*/
typedef struct t_return {
	std::vector<std::string>	params; // not sure what these should look like
}	t_return;

	/*	location	*/
typedef struct t_location {
	std::string					prefix; // needed
	std::string					root; // needed
	std::string					index; // opt if cgi_pass is not set
	bool						autoindex; // needed
	std::map<e_method, bool>	allowed_methods { {GET, false}, {POST, false}, {DELETE, false} }; // needed
	std::string					upload_store; // opt
	t_cgi_pass					cgi_pass; // opt
	t_return					returns; // opt
}	t_location;

	/*	server	*/
typedef struct cfg_server_t {
	std::string					server_name;
	uint32_t					ip_addr = INADDR_ANY;
	std::vector<int>			ports;
	std::string					root;
	size_t						client_max_body_size;
	std::map<int, std::string>	error_pages;
	std::vector<t_location>		locations;
	t_cgi_pass					cgi_pass; // opt
	bool						autoindex;
}	cfg_server_t;
/*
*/

class	Config {
public:
	bool	is_correct = false;
	std::vector<cfg_server_t>	servers;
};

int							parseConfig(const ParsingInfo parsing_info);
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