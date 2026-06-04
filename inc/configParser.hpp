/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParser.hpp                                        :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 13:16:53 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/03 15:50:18 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

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
#define SHOW_DEBUG true

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
typedef struct t_cgi_pass {
	std::vector<std::string>	params; // not sure what these should look like
}	t_cgi_pass;

	/*	return	*/
typedef struct t_return {
	std::vector<std::string>	params; // not sure what these should look like
}	t_return;

	/*	location	*/
typedef struct t_location {
	std::string				prefix;
	std::string				root;
	std::string				index;
	bool					autoindex;
	std::vector<e_method>	allowed_methods;
	std::string				upload_store;
	t_cgi_pass				cgi_pass;
	t_return				returns;
	// djahsds
}	t_location;

	/*	server	*/
typedef struct cfg_server_t {
	struct {
		char*		host_str;
		char*		port_str;
		uint32_t	ip_addr;
		int			port;
	}	listen;
	std::string					root;
	size_t						client_max_body_size;
	std::vector<t_error_page>	error_pages;
	std::vector<t_location>		locations;
	bool						autoindex;
	//	"cgi_pass", - need that here as well
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
