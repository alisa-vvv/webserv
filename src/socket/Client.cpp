/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:18 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/28 11:13:38 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Listener.hpp"
#include <ctime>

/* ============================ CONSTRUCTORS ============================ */
Client::Client() :
	_listenerFd(-1),
	_clientFd(-1),
	_rcvBuffer(),
	_lastActivity(setTimer()),
	_listener(NULL),
	_recvStatus(INCOMPLETE),
	_response(),
	_responseStatus(false),
	_bytesSent(0),
	_http()
{
}

Client::Client(const Listener *listener, int clientFd) :
	_clientFd(clientFd),
	_rcvBuffer(),
	_lastActivity(setTimer()),
	_listener(listener),
	_recvStatus(INCOMPLETE),
	_response(),
	_responseStatus(false),
	_bytesSent(0),
	_http()

{
	if (listener != NULL)
		_listenerFd = listener->getListenerFd();
	else
		_listenerFd = -1;
}

Client ::~Client()
{
}

/* ============================== GETTERS ============================== */

time_point<system_clock> Client::getLastActivity() const
{
	return this->_lastActivity;
}

receiveStatus Client::getRecvStatus() const
{
	return (_recvStatus);
}

const std::string &Client::getResponse() const
{
	return _response;
}

bool Client::getResponseStatus() const
{
	return _responseStatus;
}

size_t Client::getBytesSent() const
{
	return _bytesSent;
}

int Client::getListenerFd() const
{
	return this->_listenerFd;
}

RcvBuffer &Client::getRcvBuffer()
{
	return this->_rcvBuffer;
}

const Listener*	Client::getListenerClass() const
{
	return _listener;
}

int Client::getClientFd() const
{
	return _clientFd;
}

Http &Client::getHttpClass()
{
	return _http;
}

/* ============================== SETTERS ============================== */

void Client::setLastActivity(time_point<system_clock> lastActivity)
{
	_lastActivity = lastActivity;
}

void Client::setRecvStatus(receiveStatus recvStatus)
{
	this->_recvStatus = recvStatus;
}

void  Client::setResponse(const std::string& response)
{
	_response = response;
	_bytesSent = 0;
	_responseStatus = true;
}

void Client::setResponseStatus(bool response)
{
	_responseStatus = response;
}

void Client::setHttpClass(Http http)
{
	this->_http = http;
}


/* ======================== RESPONSE HANDLING ========================= */
void Client::updateBytesSent(size_t bytes)
{
	_bytesSent += bytes;
}
bool Client::isResponseComplete() const
{
	return _bytesSent == _response.size();
}

/* ========================== TIMEOUT HANDLING ========================== */
void Client::updateLastActivity()
{
	_lastActivity = setTimer();
}
