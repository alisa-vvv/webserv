/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 20:52:11 by tutku             #+#    #+#             */
/*   Updated: 2026/07/22 14:09:23 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// std::vector<Listener>	_listeners;
void Server::_buildListener()
{
	for (size_t i = 0; i < _config.servers.size(); i++)
	{
		const cfg_server_t &serverConfig = _config.servers[i];

		for (size_t j = 0; j < serverConfig.ports.size(); j++)
		{
			uint32_t ip = serverConfig.ip_addr;
			int port = serverConfig.ports[j];

			if (matchConfig(ip, port, serverConfig) != 1)
			{
				Listener listenerTemp;

				listenerTemp.setIpAddr(ip);
				listenerTemp.setPort(port);
				listenerTemp.addServerConfig(&serverConfig);
				_listeners.push_back(listenerTemp);
			}
		}
	}
}

int Server::matchConfig(uint32_t ip, int port, const cfg_server_t &serverConfig)
{
	for (size_t i = 0; i < _listeners.size(); i++)
	{
		if ((_listeners[i].getIpAddr() == ip) &&(_listeners[i].getPort() == port))
		{
			_listeners[i].addServerConfig(&serverConfig);
			return 1;
		}
	}
	return 0;
}

const Listener *Server::_findListenerByFd(int fd)
{
	for (size_t i = 0; i < _listeners.size(); ++i)
	{
		if (_listeners[i].getListenerFd() == fd)
			return &_listeners[i];
	}
	return NULL;
}

eServerError Server::_setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == ERROR)
	{
		std::cerr << "Error fcntl F_GETFL: " << std::strerror(errno) << std::endl;
		return SERVER_SETNONBLOCKING_ERR;
	}
	int status = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (status == ERROR)
	{
		std::cerr << "Error making fd non-blocking: " << std::strerror(errno) << std::endl;
		return SERVER_SETNONBLOCKING_ERR;
	}
	return SERVER_OK;
}

// test
// https://man7.org/linux/man-pages/man2/poll.2.html
void Server::printPollInfo(int i) // @alisa commented this out cause it output
{
	(void) i;
	//std::cout << "fd " << _pollFds[i].fd << " revents: ";
	//if (_pollFds[i].revents & POLLIN) // There is data to read.
	//	std::cout << "POLLIN ";
	//if (_pollFds[i].revents & POLLOUT) // Writing is now possible
	//	std::cout << "POLLOUT ";
	//if (_pollFds[i].revents & POLLHUP) // Hang up
	//	std::cout << "POLLHUP ";
	//if (_pollFds[i].revents & POLLERR) // Error condition
	//	std::cout << "POLLERR ";
	//if (_pollFds[i].revents & POLLNVAL) // Invalid request, fd not open
	//	std::cout << "POLLNVAL ";
	//std::cout << std::endl;
}
