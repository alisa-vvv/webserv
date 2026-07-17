/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_parser_main.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 14:18:31 by avaliull          #+#    #+#             */
/*   Updated: 2026/07/17 11:08:16 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include "cgi_exec.hpp"
#include <sys/types.h>

#include <netdb.h>

int	main(int argc, char** argv) {
	if (argc != 2)
		return (1);
	char* config_file_path = argv[1]; // path to config file
	std::optional<Config>	parse_config_ret = parseConfig(config_file_path);
	if (parse_config_ret == std::nullopt) { // error during parsing.
		return (1);
	}
	const Config& config = *parse_config_ret;

	std::vector<cgi_t>		cgis_in_background;
	std::optional<cgi_t>	cgi = executeCGI(config.servers.back());
	if (cgi == std::nullopt) {
		// brr brr error
		return (1);
	}
	cgis_in_background.push_back(*cgi);
}
