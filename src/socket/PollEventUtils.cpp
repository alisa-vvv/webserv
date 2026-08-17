/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollEventUtils.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:55:10 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/08/17 17:27:13 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::_isListenerFd(int fd) const
{
	for (size_t i = 0; i < _listeners.size(); i++)
	{
		if (_listeners[i].getListenerFd() == fd)
			return true;
	}
	return false;
}

bool Server::_isClientFd(int fd) const
{
	if (_clients.find(fd) != _clients.end())
		return true;
	return false;
}

bool Server::_isCgiFd(int fd)
{
	if (_isListenerFd(fd) || _isClientFd(fd))
		return false;
	return true;
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

void Server::closeForNow(int fd)
{
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (fd == _pollFds[i].fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}
	close(fd);
}

eClientEventResult Server::_handleCgiEvent(int cgiFd, int i)
{
	int clientFd = _cgiFdToClientFd[pollFd];
	int err = checkCgiDone(_activeCgis.at(pollFd)); // @alisa: I changed this to int cause I don't want to include the Server header in my stuff just fot the error type.

	//cgi_t& cgi = _activeCgis.at(poll);
	if (err == -1)
	{
		_closeClientFd(clientFd);
		return CLIENT_REMOVED;
	}
	if (err == 0 && _clients.at(clientFd).getHttpClass().getState() == READY_TO_SEND)
	{
		
	}

	//if (_clients.at(clientFd).getHttpClass().getState() == READY_TO_SEND)
	//{

	//}

}

void Server::_removeActiveCgi(int cgiFd)
{
	_removeFdFromPoll(cgiFd);
	close(cgiFd); //is it implemented on alisa's side?

	size_t removed = _activeCgis.erase(cgiFd);
	if (removed == 0)
		std::cerr << "CGI fd was not found\n";
}
