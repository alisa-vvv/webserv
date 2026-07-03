/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 20:52:11 by tutku             #+#    #+#             */
/*   Updated: 2026/07/03 16:45:07 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// std::vector<Listener>	_listeners;
void Server::_buildListener()
{
	for (size_t i = 0; i < _config.servers.size(); i++)
	{
		cfg_server_t serverConfig = _config.servers[i];

		for (size_t j = 0; j < serverConfig.ports.size(); j++)
		{
			uint32_t ip = serverConfig.ip_addr;
			int port = serverConfig.ports[j];

			if (matchConfig(ip, port, serverConfig) != 1)
			{
				Listener listenerTemp;

				listenerTemp.setIpAddr(_config.servers.at(i).ip_addr);
				listenerTemp.setPort(_config.servers.at(i).ports[j]);
				listenerTemp.setServerConfig(&(serverConfig));
				_listeners.push_back(listenerTemp);
			}
		}
	}
}

int Server::matchConfig(uint32_t ip, int port, cfg_server_t serverConfig)
{
	for (size_t i = 0; i < _listeners.size(); i++)
	{
		if ((_listeners[i].getIpAddr() == ip) &&(_listeners[i].getPort() == port))
		{
			_listeners[i].setServerConfig(&(serverConfig));
			return 1;
		}
	}
	return 0;
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

/*
remove clientFd from _pollFds
erase clientFd from _clients
*/
void Server::_closeClientFd(int fd)
{

	for (int i = 0; i < (int)(_pollFds.size()); i++)
	{
		if (fd == _pollFds[i].fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}
	_clients.erase(fd);
	close(fd);
}


void Server::_closeClients()
{

}

void Server::_closeAll()
{
	_closeClients();
	closeListeners();
	
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
