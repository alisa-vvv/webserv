/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollEventUtils.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 13:55:10 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/08/25 15:20:14 by tcakir-y         ###   ########.fr       */
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
		_printDebug("[LISTENER ERROR]", serverListenFd, "listener not found", true);
		return SERVER_LISTENER_NOT_FOUND_ERR;
	}

	while (1)
	{
		int clientFd = accept(serverListenFd, NULL, NULL);

		if (clientFd >= 0)
		{
			if (_setNonBlocking(clientFd) != SERVER_OK)
			{
				_printDebug("[CLIENT ERROR]", clientFd, "failed to set non-blocking mode", true);

				close(clientFd);
				continue;
			}

			Client newClient(listener, clientFd);
			_clients[clientFd] = newClient;
			
			_addFdToPoll(clientFd);
			_printDebug("[ACCEPT]", newClient, "connected", false);
			std::cout << std::endl;
			continue;
		}
		else if (errno == EWOULDBLOCK || errno == EAGAIN)
			break;
		if (errno == EINTR || errno == ECONNABORTED)
			continue;

		const std::string infoMsg = "accept failed: " + std::string(std::strerror(errno));
		_printDebug("[ACCEPT ERROR]", serverListenFd, infoMsg, true);

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

	const std::string infoMsg = "bytes=" + std::to_string(response.size());
	_printDebug("[CGI RESPONSE]", _clients.at(clientFd), cgiFd, infoMsg, false);

	_removeActiveCgi(cgiFd);
}

void Server::_removeActiveCgi(int cgiFd)
{
	_removeFdFromPoll(cgiFd);
	// close(cgiFd); //is it implemented on alisa's side?

	size_t removed = _backgroundCgis.erase(cgiFd);
	if (removed == 0)
		_printDebug("[CGI REMOVE ERROR]", cgiFd, "CGI fd not found", true);
	_cgiFdToClientFd.erase(cgiFd);
}

// std::map<int, cgi_t> _backgroundCgis; // cgiFd -> CGI data
// std::map<int, int> _cgiFdToClientFd;  // cgiFd -> clientFd
eServerError Server::_startCgi(int clientFd)
{
	Client &client = _clients.at(clientFd);

	std::optional<cgi_t> cgi = executeCGI(client, _backgroundCgis);

	if (!cgi.has_value())
	{
		_printDebug("[CGI START ERROR]", client, "failed to start CGI", true);
		return SERVER_CGI_ERR;
	}

	int cgiFd = cgi->output;
	_cgiFdToClientFd[cgiFd] = clientFd;
	_addFdToPoll(cgiFd);

	_printDebug("[CGI START]", client, cgiFd, "", false);

	return SERVER_OK;
}
