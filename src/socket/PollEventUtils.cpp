/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollEventUtils.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:55:10 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/22 16:08:45 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int Server::_isListenerFd(int fd) const
{
	for (size_t i = 0; i < _listeners.size(); i++)
	{
		if (_listeners[i].getListenerFd() == fd)
			return 1;
	}
	return 0;
}

bool Server::_isCgiEvent(int fd)
{
	return (getActiveCgis().find(fd) != getActiveCgis().end());
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

eClientEventResult Server::_handleCgiEvent(int pollFd, int i)
{
	int clientFd = _activeCgis.at(pollFd).getclientFd();
	//eServerError err = callCGI();
	if (err != SERVER_OK)
	{
		closeForNow(fd); //todo:finish
		return CLIENT_REMOVED;
	}
	if (_clients.at(clientFd).getClientState() == HANDLING_CGI_EXTENSION)
	{
		return CLIENT_KEPT;
	}
	if (_pollFds[i].revents & POLLOUT)
	{
		eServerError err = _handleSend(_clients.at(clientFd)); // change client into _activeCgis.at(fd).getClient()
		if (err != SERVER_OK)
		{
			closeForNow(fd);//todo:finish
			return CLIENT_REMOVED;
		}
		if (_clients.at(clientFd).isResponseComplete()) //check if the response is complete
		{
			std::cout << "Response completely sent to client "
					<< fd << std::endl;//todo:finish
			closeForNow(fd);//todo:finish
			return CLIENT_REMOVED;
		}
	}
	//todo:finish
	if (_clients.at(fd).getResponseStatus() && _clients.at(clientFd).getClientState() == READY_TO_SEND) //TODO:check
	{
		_pollFds[i].events = POLLOUT;
		//call close client
		return CLIENT_KEPT;
	}
	return CLIENT_KEPT; //TODO:check later

}

