/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:18 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/17 12:56:05 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Listener.hpp"
#include <ctime>

Client::Client() : _listenerFd(-1), _clientFd(-1), _lastActivity(time(NULL)), _listener(NULL)
{
}

Client::Client(const Listener *listener, int clientFd) : _clientFd(clientFd), _lastActivity(time(NULL)), _listener(listener)
{
	_listenerFd = listener->getListenerFd();
}

Client ::~Client()
{
}

time_t	Client::getLastActivity()
{
	return this->_lastActivity;
}

int Client::getListenerFd()
{
	return this->_listenerFd;
}

RcvBuffer& Client::getRcvBuffer()
{
	return this->_rcvBuffer;
}

const Listener*	Client::getListenerClass() const
{
	return _listener;
}

void	Client::setLastActivity(time_t lastActivity)
{
	_lastActivity = lastActivity;
}

void	Client::updateLastActivity()
{
	_lastActivity = time(NULL);
}
