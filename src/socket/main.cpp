/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 19:57:27 by tutku             #+#    #+#             */
/*   Updated: 2026/05/10 22:52:35 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Error! No port provided" << std::endl;
		return (ERROR);
	}
	
	ServerSocket server;
	if (server.setup() == ERROR)
	{
		return (1);
	}

	close(server.get_fd()); //TODO:change later
	return (0);
}
