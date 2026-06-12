/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParsingInfo.hpp                                   :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/02 18:15:08 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/12 19:10:30 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "configParser.hpp"
#include <string>
#include <vector>

#define CLIENT_MAX_BODY_SIZE 1000

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
/**/

// Contains vectors with allowed block names.
// Each allowed block name corresponds to a function that fills the corresponding
// values inside a Config instance.
class	ParsingInfo {
public:
	const std::vector<std::string>	global_valid_block_names {
		"server",
	};
	const std::vector<tokenParserFnPtr_t>	global_matching_functions {
		(tokenParserFnPtr_t) fillServerField,
	};

	// CHANGE THESE THINGS TO MAPS

	const std::vector<std::string>	server_valid_block_names {
		"server_name",
		"listen",
		"root",
		"location",
		"error_page",
		"client_max_body_size",
		"cgi_pass",
		"autoindex",
	};
	const std::vector<tokenParserFnPtr_t>	server_matching_functions {
		fillServerNameField,
		fillListenField,
		fillServerRootField,
		fillServerLocationField,
		fillServerErrorPageField,
		fillServerMaxBodySize,
		fillServerCgiPass,
		fillServerAutoIndex,
	};

	const std::vector<std::string>	location_valid_block_names {
		"root",
		"index",
		"allowed_methods",
		"upload_store",
		"cgi_pass",
		"autoindex",
		"return",
	};
	const std::vector<tokenParserFnPtr_t>	location_matching_functions {
		fillLocationRootField,
		fillLocationIndexField,
		fillLocationAllowedMethodsField,
		fillLocationUploadStoreField,
		fillLocationCgiPass,
		fillLocationAutoIndex,
	};

	const std::vector<std::string>	method_valid_names {
		"GET",
		"POST",
		"DELETE",
	};
};
