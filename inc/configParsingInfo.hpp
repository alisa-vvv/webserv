/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   configParsingInfo.hpp                                   :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/02 18:15:08 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/03 15:45:53 by avaliull            ########   odam.nl   */
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
typedef	bool (*tokenParserFnPtr_t)(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
);
bool fillListenField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
);
bool fillServerRootField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
);
bool fillServerField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
);
bool fillServerLocationField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
);
bool fillServerErrorPageField(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
);
bool fillServerMaxBodySize(
	Config& config,
	[[maybe_unused]] const ParsingInfo& parsing_info,
	[[maybe_unused]] const size_t& token_index,
	[[maybe_unused]] std::vector<t_config_token>& tokens
);

// Contains vectors with allowed block names.
// Each allowed block name corresponds to a function that fills the corresponding
// values inside a Config instance.
class	ParsingInfo {
public:
	const std::vector<std::string>	depth0_valid_block_names {
		"server",
	};
	const std::vector<tokenParserFnPtr_t>	depth_0_matching_functions {
		(tokenParserFnPtr_t) fillServerField,
	};

	const std::vector<std::string>	server_valid_block_names {
		"listen",
		"root",
		"location",
		"error_page",
		"client_max_body_size",
		"cgi_pass",
		"autoindex",
	};
	const std::vector<tokenParserFnPtr_t>	server_matching_functions {
		(tokenParserFnPtr_t) fillListenField,
		(tokenParserFnPtr_t) fillServerRootField,
		(tokenParserFnPtr_t) fillServerLocationField,
		(tokenParserFnPtr_t) fillServerErrorPageField,
		(tokenParserFnPtr_t) fillServerMaxBodySize,
	};

	const std::vector<std::string>	location_valid_block_names {
		"root",
		"index",
		"allowed_methods",
		"upload_store",
		"cgi_pass",
		"return",
		"autoindex",
	};
	const std::vector<std::string>	method_valid_names {
		"GET",
		"POST",
		"DELETE",
	};
};
