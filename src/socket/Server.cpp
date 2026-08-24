/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:35 by tutku             #+#    #+#             */
/*   Updated: 2026/08/24 19:00:51 by tutku            ###   ########.fr       */
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

	_printSection("SETTING UP LISTENERS");
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

	_printSection("INITIALIZING POLL");
	_addListenerFdsToPoll();
	if (_pollFds.empty()) //server has nothing to listen on
	{
		_printDebug("[POLL INIT ERROR]", "no listener fds available", true);
		return SERVER_POLL_ERR;
	}

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
			_printDebug("[TIMEOUT]", _clients.at(clientFd), "client inactive for too long", true);
			_closeClientFd(clientFd);
		}
	}
}
const std::map<int, cgi_t>& Server::getActiveCgis() const //TODO:finish
{
	return _backgroundCgis;
}

Server::Server(const Config &config) : _config(config)
{
}

Server::~Server()
{
}
