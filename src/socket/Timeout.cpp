/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timeout.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:25:45 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/08/27 10:02:23 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::_buildTimeoutResponse(Http& http, int timeoutCode)
{
	http.setResponseCode(timeoutCode);
	
	http.handleErrorResponse();

	http.setResponseHeader("Content-Length", std::to_string(http.getBody().size()));
	http.setResponseHeader("Connection", "close");
	
	http.buildResponseString();
	http.setState(READY_TO_SEND);
}

void Server::_checkClientTimeouts()
{
	std::map<int, Client>::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		const int clientFd = it->first;
		const time_point<system_clock> lastActivity = it->second.getLastActivity();
		
		++it;

		Client &client = _clients.at(clientFd);
		Http &http = client.getHttpClass();

		if (checkTimeOut(lastActivity, DEFAULT_TIMEOUT_S))
		{
			if (client.getRecvStatus() != COMPLETE) //still receiving but gave timeout
			{

				_sendTimeoutResponse(client, HTTP_REQUEST_TIMEOUT);
				printDebug("[TIMEOUT]", client, "cannot receive from the client", true);
			}
			else if (http.getState() == HANDLING_CGI_EXTENSION ||
					http.getState() == HANDLING_CGI_STATIC)
			{
				continue;
			}
			else if (http.getState() == READY_TO_SEND && !client.isResponseComplete()) //Sending response stuck
			{
				printDebug("[TIMEOUT]", client, "sending response stuck for too long", true);
				_closeClientFd(clientFd);
			}
			else
			{
				printDebug("[TIMEOUT]", client, "client inactive for too long", true);
				_closeClientFd(clientFd);
			}
		}
	}
}

void Server::_checkCgiTimeouts()
{
	for (size_t i = 0; i < _pollFds.size(); i++) 
	{
		const int fd = _pollFds.at(i).fd;
		if (_isCgiFd(fd)) {
			std::map<int, cgi_t>& cgi_map = getActiveCgis();
			checkCgiTimeout(cgi_map.at(fd));
		}
	}
}

void Server::_checkTimeouts()
{
	_checkClientTimeouts();
	_checkCgiTimeouts();
}

void Server::_sendTimeoutResponse(Client &client, int timeoutCode)
{
	Http &http = client.getHttpClass();

	_buildTimeoutResponse(http, timeoutCode);

	client.setResponse(http.getResponseString());
	client.setResponseStatus(true);
	client.updateLastActivity();

	_setPollout(client.getClientFd());
}


/*
CLIENT TIMEOUTS
├── Receiving request stuck → 408
├── Sending response stuck   → close
└── Idle connection          → close -> no recv no send

CGI TIMEOUT
└── CGI execution stuck      → 504
*/