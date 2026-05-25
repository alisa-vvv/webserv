/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 19:57:27 by tutku             #+#    #+#             */
/*   Updated: 2026/05/25 16:50:22 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"

int main (int argc, char *argv[])
{
	(void) argv;
	if (argc < 2)
	{
		std::cerr << "Error! No config file provided" << std::endl;
		return (ERROR);
	}
	ServerSocket server;
	if (server.setup() == ERROR)
	{
		return (1);
	}
	std::cout << "Socket created, started listening" << std::endl;
	return (0);
}
