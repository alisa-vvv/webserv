/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 20:52:11 by tutku             #+#    #+#             */
/*   Updated: 2026/08/24 19:03:49 by tutku            ###   ########.fr       */
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
		const std::string infoMsg ="F_GETFL failed: " + std::string(std::strerror(errno));
		_printDebug("[FCNTL ERROR]", fd, infoMsg, true);
		return SERVER_SETNONBLOCKING_ERR;
	}
	int status = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (status == ERROR)
	{
		const std::string infoMsg = "F_SETFL failed: " + std::string(std::strerror(errno));
		_printDebug("[FCNTL ERROR]", fd, infoMsg, true);
		return SERVER_SETNONBLOCKING_ERR;
	}
	_printDebug("[NONBLOCKING]", fd, "enabled", false);
	return SERVER_OK;
}
