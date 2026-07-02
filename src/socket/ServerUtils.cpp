/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 20:52:11 by tutku             #+#    #+#             */
/*   Updated: 2026/07/02 23:10:23 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// std::vector<Listener>	_listeners;
void Server::_buildListener()
{
	for (size_t i = 0; i < _config.servers.size(); i++)
	{
		for (size_t j = 0; j < _config.servers[i].ports.size(); j++)
		{
			Listener listenerTemp;

			listenerTemp.setIpAddr(_config.servers.at(i).ip_addr);
			listenerTemp.setPort(_config.servers.at(i).ports[j]);
			_listeners.push_back(listenerTemp);
		}
	}
}

void Server::_addFdToPoll(int fd)
{
	struct pollfd pollFd;

	pollFd.fd = fd;
	pollFd.events = POLLIN;
	pollFd.revents = 0;

	_pollFds.push_back(pollFd);
}

// Add listener fds to the list of fds that poll() should watch
void Server::_addListenerFdsToPoll()
{
	for (size_t i = 0; i < _listeners.size(); i++)
	{
		_addFdToPoll(_listeners[i].getListenerFd());
	}
}

int Server::_isListenerFd(int fd) const
{
	for (size_t i = 0; i < _listeners.size(); i++)
	{
		if (_listeners[i].getListenerFd() == fd)
			return 1;
	}
	return 0;
}

//TODO:fix and finish
eServerError Server::_acceptClients(int serverListenFd)
{
	while (1)
	{
		int clientFd = accept(serverListenFd, NULL, NULL);

		if (clientFd >= 0)
		{
			if (_setNonBlocking(clientFd) != SERVER_OK)
			{
				close(clientFd);
				continue;
			}
			_addFdToPoll(clientFd);

			Client newClient(clientFd);
			_clients[clientFd] = newClient;
			continue;
		}
		else if (errno == EWOULDBLOCK || errno == EAGAIN)
			break;
		if (errno == EINTR || errno == ECONNABORTED) // check accept() man
			continue;
		std::cerr << "Error accept(): " << std::strerror(errno) << std::endl;
		return SERVER_ACCEPT_ERR;
	}
	return SERVER_OK;
}

void Server::closeListeners()
{
	for (size_t i = 0; i < _listeners.size(); i++)
	{
		if (_listeners[i].getListenerFd() != -1)
		{
			close(_listeners[i].getListenerFd());
			_listeners[i].setListenerFd(-1);
		}
	}
}

// test
// https://man7.org/linux/man-pages/man2/poll.2.html
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
