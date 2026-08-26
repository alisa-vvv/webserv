/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCleanup.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 14:00:29 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/08/26 14:12:25 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


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
	_cleanupCgiForClient(fd);

	_removeFdFromPoll(fd);
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

void Server::_removeFdFromPoll(int fd)
{
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (fd == _pollFds[i].fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}
}

void Server::_closeAll()
{
	_closeClients();
	closeListeners();
}

void Server::_cleanupCgiForClient(int clientFd)
{
	std::map<int, int>::iterator it = _cgiFdToClientFd.begin();

	while (it != _cgiFdToClientFd.end())
	{
		if (it->second == clientFd)
		{
			int cgiFd = it->first;

			if (_backgroundCgis.find(cgiFd) != _backgroundCgis.end())
			{
				killCgi(_backgroundCgis.at(cgiFd));
				_removeFdFromPoll(cgiFd);
				_backgroundCgis.erase(cgiFd);
			}
			_cgiFdToClientFd.erase(it);
			break;
		}
		++it;
	}
}
