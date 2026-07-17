/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:18 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/17 16:48:08 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Listener.hpp"
#include <ctime>

Client::Client() : _listenerFd(-1), _clientFd(-1), _lastActivity(time(NULL)), _listener(NULL)
{
	_responseStatus = false;

}

Client::Client(const Listener *listener, int clientFd) : _clientFd(clientFd), _lastActivity(time(NULL)), _listener(listener)
{
	_listenerFd = listener->getListenerFd();
	_responseStatus = false;

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

void Client::setRecvStatus(receiveStatus recvStatus)
{
	this->_recvStatus = recvStatus;
}

receiveStatus Client::getRecvStatus() const
{
	return (_recvStatus);
}

void  Client::setResponse(std::string response)
{
	_response = response;
}
std::string Client::getResponse() const
{
	return _response;
}

const char* Client::updatedResponse(std::string response)
{
	const char *responseChar = response.c_str();
	return responseChar;
}

void  Client::setResponseStatus(bool response)
{
	_responseStatus = response;
}
bool Client::getResponseStatus() const
{
	return _responseStatus;
}

int Client::getClientFd()
{
	return _clientFd;
}