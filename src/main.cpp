/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 19:57:27 by tutku             #+#    #+#             */
/*   Updated: 2026/07/17 11:08:58 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main (int argc, char *argv[])
{
	(void)argv;
	if (argc != 2)
	{
		std::cerr << "Error! No config file provided" << std::endl;
		return (ERROR);
	}

	if (setupSignal() != SERVER_OK)
		return (ERROR);
	
	char* config_file_path = argv[1]; // path to config file
	std::optional<Config>	parse_config_ret = parseConfig(config_file_path);
	if (parse_config_ret == std::nullopt)  // error during parsing.
		return (ERROR);
	const Config& config = *parse_config_ret;

	Server server(config);
	eServerError err = server.setup();
	if (err != SERVER_OK)
	{
		std::cerr << "Server setup failed" << std::strerror(errno) <<std::endl;
		server.closeListeners();
		return (1);
	}
	err = server.run();
	if (err != SERVER_OK)
	{
		std::cerr << "Server run failed" << std::strerror(errno) <<  std::endl;
		server.closeListeners();
		return (1);
	}
	server.closeListeners();
	std::cout << "Socket created, started listening" << std::endl;
	return (0);
}

/*
flow:

socket()
setsockopt()
fcntl(non-blocking)
setAddress()
bind()
listen()
*/

/*
//cgi order
	std::vector<cgi_t>		cgis_in_background;
	std::optional<cgi_t>	cgi = executeCGI(config.servers.back());
	if (cgi == std::nullopt) {
		// brr brr error
		return (1);
	}
	cgis_in_background.push_back(*cgi);
*/
