/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:18 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/14 16:23:55 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <ctime>

Client::Client() //set fd=-1??
{
}

Client::Client(int listenerFd, int clientFd) : _listenerFd(listenerFd), _clientFd(clientFd), _lastActivity(time(NULL))
{
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

void	Client::setLastActivity(time_t lastActivity)
{
	_lastActivity = lastActivity;
}

void	Client::updateLastActivity()
{
	_lastActivity = time(NULL);
}
