/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   ParsingInfo.hpp                                         :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/02 18:15:08 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/02 19:48:12 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>

class	ParsingInfo {
public:
	const std::vector<std::string>	depth0_valid_block_names {
		"server",
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
