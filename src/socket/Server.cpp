/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:35 by tutku             #+#    #+#             */
/*   Updated: 2026/07/22 13:54:02 by tcakir-y         ###   ########.fr       */
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

Server::Server(const Config &config) : _config(config)
{
}

Server::~Server()
{
}
