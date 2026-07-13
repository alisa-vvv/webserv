/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   config_parser_main.cpp                                  :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 14:18:31 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/25 13:01:45 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "cgi_exec.hpp"

int	main(int argc, char** argv) {
	if (argc != 2)
		return (1);
	char* config_file_path = argv[1]; // path to config file
	std::optional<Config>	parse_config_ret = parseConfig(config_file_path);
	if (parse_config_ret == std::nullopt) { // error during parsing.
		return (1);
	}
	const Config& config = *parse_config_ret;
	executeCGI(config.servers.back());
}
