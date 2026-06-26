/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 19:57:27 by tutku             #+#    #+#             */
/*   Updated: 2026/06/26 15:30:44 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Error! No config file provided" << std::endl;
		return (ERROR);
	}

	if (setupSignal() != SERVER_OK)
	{
		return 1;
	}
	
	//TODO: config comes here
	Config config;
	Server server(config);
	eServerError err = server.setup();
	if (err != SERVER_OK)
	{
		std::cerr << "Server setup failed" << std::endl;
		return (1);
	}
	err = server.run();
	if (err != SERVER_OK)
	{
		return (1);
	}
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
