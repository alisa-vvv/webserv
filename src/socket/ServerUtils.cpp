/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 20:52:11 by tutku             #+#    #+#             */
/*   Updated: 2026/07/20 01:23:36 by tutku            ###   ########.fr       */
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

eServerError Server::_acceptClients(int serverListenFd)
{
	const Listener *listener = _findListenerByFd(serverListenFd);

	if (listener == NULL)
	{
		std::cerr << "Listener not found for fd "
				  << serverListenFd << std::endl;
		return SERVER_LISTENER_NOT_FOUND_ERR;
	}

	while (1)
	{
		int clientFd = accept(serverListenFd, NULL, NULL);

		if (clientFd >= 0)
		{
			if (_setNonBlocking(clientFd) != SERVER_OK)
			{
				std::cerr << "Failed to set client "
						  << clientFd
						  << " to non-blocking mode"
						  << std::endl;
				close(clientFd);
				continue;
			}

			Client newClient(_findListenerByFd(serverListenFd), clientFd);
			_clients[clientFd] = newClient;
			
			_addFdToPoll(clientFd);
			std::cout << "Accepted client "
					  << clientFd
					  << " on listener "
					  << serverListenFd
					  << std::endl;
			continue;
		}
		else if (errno == EWOULDBLOCK || errno == EAGAIN)
			break;
		if (errno == EINTR || errno == ECONNABORTED)
			continue;
		std::cerr << "accept() failed on listener "
				  << serverListenFd
				  << ": "
				  << std::strerror(errno)
				  << std::endl;
		return SERVER_ACCEPT_ERR;
	}
	return SERVER_OK;
}

/**
 * @brief Closes all active listener sockets.
 *
 * @details
 * Iterates through the `_listeners` vector and closes each listener file
 * descriptor if it is valid. After closing a listener fd, its value is set
 * to `-1` to mark it as closed and avoid accidentally closing the same fd
 * again later.
 *
 * Listener fds are the sockets used to accept new client connections.
 * This function does not close client connections; clients should be closed
 * separately with `_closeClients()`.
 *
 * @note
 * This function is usually called during server shutdown or when listener
 * setup fails and the server needs to clean up already-opened sockets.
 */
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

/**
 * @brief Closes and removes a single client connection.
 *
 * @details
 * Removes the given client file descriptor from `_pollFds` so `poll()` no longer
 * watches it. Then removes the matching client from the `_clients` map and
 * closes the actual file descriptor.
 *
 * @param fd The client file descriptor that should be removed and closed.
 */
void Server::_closeClientFd(int fd)
{
	for (size_t i = 0; i < _pollFds.size(); i++)
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

/**
 * @brief Closes all active client connections.
 *
 * @details
 * Iterates through the `_clients` map and closes every client file descriptor.
 * Each client is closed by calling `_closeClientFd()`, which removes the fd
 * from `_pollFds`, erases the client from `_clients`, and closes the actual fd.
 *
 * The loop continues until `_clients` is empty. We always take the first client
 * fd from the map because `_closeClientFd()` modifies `_clients` by erasing
 * the current client.
 */
void Server::_closeClients()
{
	int fd;

	while (_clients.empty() == 0)
	{
		fd = _clients.begin()->first;
		_closeClientFd(fd);
	}
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
