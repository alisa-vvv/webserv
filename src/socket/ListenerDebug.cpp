/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenerDebug.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 11:57:12 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/02 20:54:40 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"

void Listener::printPortNumber()
{
	socklen_t length = sizeof(_address);

	if (getsockname(_listenerFd, (struct sockaddr *)&_address, &length))
	{
		perror("getting socket name");
		exit(1);
	}
	printf("Socket has port #%d\n", ntohs(_address.sin_port));
}

int Listener::_printSocketName()
{
	struct sockaddr_in addr;
	socklen_t len;

	memset(&addr, 0, sizeof(addr));
	len = sizeof(addr);

	if (getsockname(_listenerFd, reinterpret_cast<struct sockaddr *>(&addr), &len) == ERROR)
	{
		std::cerr << "getsockname failed" << std::endl;
		return ERROR;
	}
	std::cout << "Bound port: " << ntohs(addr.sin_port) << std::endl;
	return SUCCESS;
}
