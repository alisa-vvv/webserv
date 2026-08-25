/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 19:57:27 by tutku             #+#    #+#             */
/*   Updated: 2026/08/25 12:01:41 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Error! No config file provided" << std::endl;
		return (ERROR);
	}

	if (setupSignal() != SERVER_OK)
		return (ERROR);
	
	char* config_file_path = argv[1];
	std::optional<Config>	parse_config_ret = parseConfig(config_file_path);
	if (parse_config_ret == std::nullopt)
		return (ERROR);
	const Config& config = *parse_config_ret;

	Server server(config);
	eServerError err = server.setup();
	if (err != SERVER_OK)
	{
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

	// @alisa we'll need to test this. this is to kill hanging child processes on exit
	signal(SIGQUIT, SIG_IGN);
	kill(0, SIGQUIT);

	return (0);
}

