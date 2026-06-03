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

// Contains vectors with allowed block names.
// Each allowed block name corresponds to a function that fills the corresponding
// values inside a Config instance.

class	ParsingInfo {
public:
	const std::vector<std::string>	depth0_valid_block_names {
		"server",
	};
	const std::vector<
		bool (*)(
			const ParsingInfo&,
			const size_t&,
			const std::vector<t_config_token>&
	)>	depth_0_matching_functions {
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
