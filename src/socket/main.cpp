/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 19:57:27 by tutku             #+#    #+#             */
/*   Updated: 2026/06/25 13:15:03 by tcakir-y         ###   ########.fr       */
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
	
	Server server;
	//config comes here
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
