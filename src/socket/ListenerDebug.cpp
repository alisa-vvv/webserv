/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerDebug.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 11:57:12 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/06/25 14:47:02 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"

//https://man7.org/linux/man-pages/man2/poll.2.html
void Server::printPollInfo(int i)
{
	std::cout << "fd " << _pollFds[i].fd << " revents: ";
	if (_pollFds[i].revents & POLLIN) // There is data to read.
		std::cout << "POLLIN ";
	if (_pollFds[i].revents & POLLOUT) // Writing is now possible
		std::cout << "POLLOUT ";
	if (_pollFds[i].revents & POLLHUP) // Hang up
		std::cout << "POLLHUP ";
	if (_pollFds[i].revents & POLLERR) // Error condition
		std::cout << "POLLERR ";
	if (_pollFds[i].revents & POLLNVAL) // Invalid request, fd not open
		std::cout << "POLLNVAL ";
	std::cout << std::endl;
}

void Server::printPortNumber()
{
	socklen_t length = sizeof(_address);

	if (getsockname(_fd, (struct sockaddr *)&_address, &length))
	{
		perror("getting socket name");
		exit(1);
	}
	printf("Socket has port #%d\n", ntohs(_address.sin_port));
}

int Server::_printSocketName()
{
	struct sockaddr_in addr;
	socklen_t len;

	memset(&addr, 0, sizeof(addr));
	len = sizeof(addr);

	if (getsockname(_fd, reinterpret_cast<struct sockaddr *>(&addr), &len) == ERROR)
	{
		std::cerr << "getsockname failed" << std::endl;
		return ERROR;
	}
	std::cout << "Bound port: " << ntohs(addr.sin_port) << std::endl;
	return SUCCESS;
}
