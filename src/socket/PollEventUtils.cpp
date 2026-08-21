/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollEventUtils.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:55:10 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/08/20 14:29:16 by tcakir-y         ###   ########.fr       */
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
	// if (_isListenerFd(fd) || _isClientFd(fd))
	// 	return false;
	// return true;
	return _backgroundCgis.find(fd) != _backgroundCgis.end();
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
			//std::cout << "Accepted client "
			//		  << clientFd
			//		  << " on listener "
			//		  << serverListenFd
			//		  << std::endl;
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

void Server::_copyCgiResponse(int cgiFd, int clientFd)
{
	cgi_t &cgi = _backgroundCgis.at(cgiFd);

	std::string response = buildCGIResponseString(cgi.output_string);


	_clients.at(clientFd).setResponse(response);
	_clients.at(clientFd).getHttpClass().setState(READY_TO_SEND);
	_removeActiveCgi(cgiFd);
}


eClientEventResult Server::_handleCgiEvent(int cgiFd, int i)
{
	int clientFd = _cgiFdToClientFd.at(cgiFd);

	if (_pollFds[i].revents & (POLLERR | POLLNVAL))
	{
		std::cerr << "CGI event error! fd: " << cgiFd << std::endl;

		_removeActiveCgi(cgiFd);
		_closeClientFd(clientFd);
		return CLIENT_REMOVED;
	}

	int isCgiDone = checkCgiDone(_backgroundCgis.at(cgiFd));
	
	if (isCgiDone == -1)
	{
		_removeActiveCgi(cgiFd);
		_closeClientFd(clientFd);
		return CLIENT_REMOVED;
	}
	if (isCgiDone == 0) //cgi still running
	{
		return CLIENT_KEPT;
	}

	//cgi finished and response ready
	_copyCgiResponse(cgiFd, clientFd);

	//set client to POLLOUT for sending
	for (size_t j = 0; j < _pollFds.size(); j++)
	{
		if (_pollFds[j].fd == clientFd)
		{
			_pollFds[j].events = POLLOUT;
			break;
		}
	}
	return CLIENT_REMOVED;
}

void Server::_removeActiveCgi(int cgiFd)
{
	_removeFdFromPoll(cgiFd);
	// close(cgiFd); //is it implemented on alisa's side?

	size_t removed = _backgroundCgis.erase(cgiFd);
	if (removed == 0)
		std::cerr << "CGI fd was not found\n";
	_cgiFdToClientFd.erase(cgiFd);
}

// std::map<int, cgi_t> _backgroundCgis; // cgiFd -> CGI data
// std::map<int, int> _cgiFdToClientFd;  // cgiFd -> clientFd
eServerError Server::_startCgi(int clientFd)
{
	Client &client = _clients.at(clientFd);

	std::optional<cgi_t> cgi = executeCGI(client, _backgroundCgis);

	if (!cgi.has_value())
		return SERVER_CGI_ERR;

	int cgiFd = cgi->output;
	_cgiFdToClientFd[cgiFd] = clientFd;
	_addFdToPoll(cgiFd);

	return SERVER_OK;
}
