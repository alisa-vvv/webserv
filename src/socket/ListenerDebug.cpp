/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenerDebug.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 11:57:12 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/08/26 16:12:41 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"

std::string Listener::_getSocketInfo() const
{
	struct sockaddr_in addr;
	socklen_t len;

	memset(&addr, 0, sizeof(addr));
	len = sizeof(addr);

	if (getsockname(_listenerFd, reinterpret_cast<struct sockaddr *>(&addr), &len) == ERROR)
	{
		return "";
	}

	return "port=" + std::to_string(ntohs(addr.sin_port));
}

// to print socket/bind/listen information
void Listener::printDebug(const std::string &eventStr, int fd, const std::string &infoMsg, bool error)
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
