/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:35 by tutku             #+#    #+#             */
/*   Updated: 2026/08/26 13:03:17 by tcakir-y         ###   ########.fr       */
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

const std::map<int, cgi_t>& Server::getActiveCgis() const
{
	return _backgroundCgis;
}

Server::Server(const Config &config) : _config(config)
{
}

Server::~Server()
{
}
