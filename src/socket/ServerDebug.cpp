/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerDebug.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 15:11:09 by tutku             #+#    #+#             */
/*   Updated: 2026/08/26 16:12:57 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::printDebug(const std::string &eventStr, const Client &client, const std::string &infoMsg, bool error)
{
	if (error != true)
	{
		std::cout << GREEN << eventStr
				  << RESET
				  << " listener=" << client.getListenerFd()
				  << " client=" << client.getClientFd();

		if (infoMsg.empty() != true)
		{
			std::cout << " " << infoMsg;
		}
		std::cout << std::endl;
	}
	else
	{
		std::cerr << RED << eventStr
				  << RESET
				  << " listener=" << client.getListenerFd()
				  << " client=" << client.getClientFd();

		if (infoMsg.empty() != true)
		{
			std::cerr << " " << infoMsg;
		}
		std::cerr << std::endl;
	}
}

//to print cgi events
void Server::printDebug(const std::string &eventStr, const Client &client, int cgiFd, const std::string &infoMsg, bool error)
{
	if (error != true)
	{
		std::cout << GREEN << eventStr
				  << RESET
				  << " listener=" << client.getListenerFd()
				  << " client=" << client.getClientFd()
				  << " cgi=" << cgiFd;

		if (infoMsg.empty() != true)
		{
			std::cout << " " << infoMsg;
		}
		std::cout << std::endl;
	}
	else
	{
		std::cerr << RED << eventStr
				  << RESET
				  << " listener=" << client.getListenerFd()
				  << " client=" << client.getClientFd()
				  << " cgi=" << cgiFd;

		if (infoMsg.empty() != true)
		{
			std::cerr << " " << infoMsg;
		}
		std::cerr << std::endl;
	}
}

//to print poll
void Server::printDebug(const std::string &eventStr, const std::string &infoMsg, bool error)
{
	if (error != true)
	{
		std::cout << GREEN << eventStr
				  << RESET;

		if (infoMsg.empty() != true)
		{
			std::cout << " " << infoMsg;
		}
		std::cout << std::endl;
	}
	else
	{
		std::cerr << RED << eventStr
				  << RESET;

		if (infoMsg.empty() != true)
		{
			std::cerr << " " << infoMsg;
		}
		std::cerr << std::endl;
	}
}

void Server::printDebug(const std::string &eventStr, int fd, const std::string &infoMsg, bool error)
{
	if (error != true)
	{
		std::cout << GREEN << eventStr
				  << RESET
				  << " fd=" << fd;

		if (infoMsg.empty() != true)
		{
			std::cout << " " << infoMsg;
		}
		std::cout << std::endl;
	}
	else
	{
		std::cerr << RED << eventStr
				  << RESET
				  << " fd=" << fd;

		if (infoMsg.empty() != true)
		{
			std::cerr << " " << infoMsg;
		}
		std::cerr << std::endl;
	}
}

void Server::printSection(const std::string &msg)
{
	std::cout << BLUE
			  << "==================" << msg << "=================="
			  << RESET << std::endl;
}
