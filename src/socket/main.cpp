/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 19:57:27 by tutku             #+#    #+#             */
/*   Updated: 2026/05/26 16:20:12 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main (int argc, char *argv[])
{
	(void) argv;
	if (argc < 2)
	{
		std::cerr << "Error! No config file provided" << std::endl;
		return (ERROR);
	}

	//TODO: change after receiving them from config side
	int port = 8080;
	uint32_t host = INADDR_ANY;
	
	Server server(host, port); //later this should be argv[1] for config part(?)
	if (server.setup() == ERROR)
	{
		return (1);
	}
	std::cout << "Socket created, started listening" << std::endl;
	return (0);
}

/*
Examples:
0.0.0.0   -> INADDR_ANY / 0
127.0.0.1 -> 0x7F000001
*/

/*
flow:

socket()
setsockopt()
fcntl(non-blocking)
setAddress()
bind()
listen()
*/
