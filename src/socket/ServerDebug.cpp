/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerDebug.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 15:11:09 by tutku             #+#    #+#             */
/*   Updated: 2026/08/24 18:49:28 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::_printDebug(const std::string &eventStr, const Client &client, const std::string &infoMsg, bool error)
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
void Server::_printDebug(const std::string &eventStr, const Client &client, int cgiFd, const std::string &infoMsg, bool error)
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
void Server::_printDebug(const std::string &eventStr, const std::string &infoMsg, bool error)
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

void Server::_printDebug(const std::string &eventStr, int fd, const std::string &infoMsg, bool error)
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

void Server::_printSection(const std::string &msg)
{
	std::cout << BLUE
			  << "==================" << msg << "=================="
			  << RESET << std::endl;
}
