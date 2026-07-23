/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:35 by tutku             #+#    #+#             */
/*   Updated: 2026/07/22 15:35:19 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
build Listener objs
setup every listening socket
every Listener has its own listening fd
*/
eServerError Server::setup(void)
{
	eListenerError errListenerSetup;

	_buildListener();

	for (size_t i = 0; i < _listeners.size(); i++)
	{
		errListenerSetup = _listeners[i].setup();
		if (errListenerSetup != LISTENER_OK)
		{
			closeListeners();
			return SERVER_LISTENER_SETUP_ERR;
		}
	}
	return SERVER_OK;
}

eServerError Server::run()
{
	eServerError err;

	_addListenerFdsToPoll();
	if (_pollFds.empty()) //server has nothing to listen on
		return SERVER_POLL_ERR;

	err = _initPollEvent();
	if (err != SERVER_OK)
	{
		closeListeners();
		return err;
	}
	_closeAll();
	return SERVER_OK;
}

void Server::_checkClientTimeouts()
{
	std::map<int, Client>::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		const int clientFd = it->first;
		const time_point<system_clock> lastActivity = it->second.getLastActivity();
		
		it++;
		if (checkTimeOut(lastActivity, DEFAULT_TIMEOUT_S))
		{
			std::cout << "Client with fd "
						<< clientFd
						<< " timed out"
						<< std::endl;
			_closeClientFd(clientFd);
		}
	}
}

// std::map<int, cgi_t> Server::getActiveCgis() const
// {
// 	eServerError err;
// 	size_t i = 0;

// 	while (i < _pollFds.size())
// 	{
// 		int fd = _pollFds[i].fd;

// 		if (_pollFds[i].revents == 0)
// 		{
// 			i++;
// 			continue;
// 		}
		
// 		printPollInfo(i); // test

// 		if (_isListenerFd(fd))
// 		{
// 			err = _handleListenerEvent(i);
// 			if (err != SERVER_OK)
// 				return err;
// 			i++;
// 		}
// 		else
// 		{
// 			eClientEventResult clientClosed = _handleClientEvent(i);
// 			if (clientClosed == CLIENT_KEPT)
// 				i++;
// 		}
// 	}
// 	return SERVER_OK;
// }

// this listenerfd can accept new connections
//COMMENTED OUT BY ALLY, REASON: DOUBLE DEFINITION
// eServerError Server::_handleListenerEvent(int i)
// {
// 	eServerError err;
// 	int fd = _pollFds[i].fd;

// 	if (_pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
// 	{
// 		std::cerr << "Listening socket error on fd " << fd << std::endl;
// 		return SERVER_POLL_ERR;
// 	}

// 	if (_pollFds[i].revents & POLLIN)
// 	{
// 		err = _acceptClients(fd);
// 		if (err != SERVER_OK)
// 			return err;
// 	}
// 	return SERVER_OK;
// }

//COMMENTED OUT BY ALLY, REASON: DOUBLE DEFINITION
// eClientEventResult Server::_handleClientEvent(int i)
// {
// 	int fd = _pollFds[i].fd;

// 	if (_pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
// 	{
// 		_closeClientFd(fd);
// 		return CLIENT_REMOVED;
// 	}
// 	//TODO: add cgi
// 	if (_clients.at(fd).getHttpClass().getState() == HANDLING_CGI_EXTENSION) //waiting for cgi
// 	{
		
// 	}
// 	if (_pollFds[i].revents & POLLIN)
// 	{
// 		eServerError err = _handleRecv(fd);
// 		if (err != SERVER_OK)
// 		{
// 			_closeClientFd(fd);
// 			return CLIENT_REMOVED;
// 		}
// 		if (_clients.at(fd).getHttpClass().getState() == READY_TO_SEND) //TODO:check
// 		{
// 			_pollFds[i].events = POLLOUT;
// 		}
// 	}
// 	if (_pollFds[i].revents & POLLOUT)
// 	{
// 		std::cout << "Calling send for client " << fd << std::endl;

// 		std::cout << "Generated response:\n"
// 				<< _clients.at(fd).getResponse()
// 				<< "\n--- response end ---\n";

// 		eServerError err = _handleSend(_clients.at(fd));
// 		if (err != SERVER_OK)
// 		{
// 			_closeClientFd(fd);
// 			return CLIENT_REMOVED;
// 		}
// 		if (_clients.at(fd).isResponseComplete())
// 		{
// 			std::cout << "Response completely sent to client "
// 					<< fd << std::endl;
// 			_closeClientFd(fd);
// 			return CLIENT_REMOVED;
// 		}
// 	}
// 	return CLIENT_KEPT;
// }

Server::Server(const Config &config) : _config(config)
{
}

Server::~Server()
{
}
