/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParsingInfo.hpp                                   :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/02 18:15:08 by avaliull            #+#    #+#           */
/*   Updated: 2026/08/26 13:09:08 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "configParser.hpp"
#include <string>
#include <vector>

#define CLIENT_MAX_BODY_SIZE 1000
#define RETURN_CODE_LOWEST 300
#define RETURN_CODE_HIGHEST 399

typedef enum	e_context {
	GLOBAL,
	SERVER,
	LOCATION,
}	e_context;


// Function pointer type for token parsers
typedef bool	(*tokenParserFnPtr_t)(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);

class	UniqueBlockMap {
public:
	std::map<std::string, bool> map {
		{ "prefix", false },
		{ "root", false },
		{ "client_max_body_size", false },
		{ "cgi_pass", false},
		{ "autoindex", false },
		{ "index", false },
		{ "upload_store", false },
		{ "allowed_methods", false },
		{ "return", false },
	};
private:
};

/*
 * Server block Parsers
 */
bool	fillServerNameField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillListenField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillServerRootField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillServerField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillServerLocationField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillServerErrorPageField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillServerMaxBodySize(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillServerCgiPass(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillServerAutoIndex(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);

/*
* Location block parsers
*/
bool	fillLocationRootField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillLocationIndexField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillLocationAllowedMethodsField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillLocationUploadStoreField(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillLocationCgiPass(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillLocationAutoIndex(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
bool	fillLocationReturn(
	Config& config,
	const size_t& token_index,
	std::vector<t_config_token>& tokens
);
/**/

// Contains vectors with allowed block names.
// Each allowed block name corresponds to a function that fills the corresponding
// values inside a Config instance.
class	ParsingInfo {
public:
	const std::map<std::string, tokenParserFnPtr_t>	global_block_parsers {
		{ "server", fillServerField },
	};

	const std::map<std::string, tokenParserFnPtr_t>	server_block_parsers {
		{ "server_name", fillServerNameField },
		{ "listen", fillListenField },
		{ "root", fillServerRootField },
		{ "location", fillServerLocationField },
		{ "error_page", fillServerErrorPageField },
		{ "client_max_body_size", fillServerMaxBodySize },
		{ "cgi_pass", fillServerCgiPass },
		{ "autoindex", fillServerAutoIndex },
	};

	const std::map<std::string, tokenParserFnPtr_t>	location_block_parsers {
		{ "root", fillLocationRootField },
		{ "index", fillLocationIndexField },
		{ "allowed_methods", fillLocationAllowedMethodsField },
		{ "upload_store", fillLocationUploadStoreField },
		{ "cgi_pass", fillLocationCgiPass },
		{ "autoindex", fillLocationAutoIndex },
		{ "return", fillLocationReturn },
	};

	const std::vector<std::string>	method_valid_names {
		"GET",
		"POST",
		"DELETE",
	};
};
