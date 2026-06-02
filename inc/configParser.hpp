/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParser.hpp                                        :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 13:16:53 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/02 18:16:03 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

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
//
#define CONFIG_PATH_DEFAULT "config/default.conf"
#define CONFIG_PATH_TEST "config/test.conf"

// Config format::
// name {
// 	key	value;
//
// 	key	value1
// 		value2
// 		value3;
// }
// etc.
//

typedef enum e_config_mode {
	CONFIG_MODE_TEST,
	CONFIG_MODE_DEFAULT,
	CONFIG_MODE_PASS,
}	e_config_mode;

typedef enum e_token_type {
	UNDEFINED_TYPE,
	BLOCK_NAME, // example: location
	BLOCK_PREFIX, // example: /cgi-bin
	BRACE_OPEN,
	BRACE_CLOSE,
	KEY,
	VALUE,
	SEMICOLON,
	NEWLINE,
} e_token_type;

typedef struct	s_token {
	e_token_type	type;
	std::string		val;
}	t_config_token;

class	Config {
	typedef struct cfg_server_t {
		struct {
			char*	host;
			char*	port;
		}	listen;
	}	cfg_server_t;
public:
	std::vector<cfg_server_t>	servers;
	// default errror pages;
	// maximum size allowed for client request bodies
	// rules on url route:
	// 	list of accepted HTTP methods for the rotue;
	// 	HTTP redirection;
	// 	Directory where the requested file should be located;
	// 	enable or disable directory listing;
	// 	default file to serve when the requested resource is a directory;
	// 	is uploading authorized
	// 	extension of cgi
};

int	parseConfig();
