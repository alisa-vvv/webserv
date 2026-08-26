/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timeout.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:25:45 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/08/26 13:55:07 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::_setTimeoutResponse(Http& http, int timeoutCode)
{
	http.setResponseCode(timeoutCode);
	http.handleErrorResponse();
	http.buildResponse();
	http.buildResponseString();

	std::cout << "response: " << http.getResponseString() << '\n';
}

void Server::_checkClientTimeouts()
{
	std::map<int, Client>::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		const int clientFd = it->first;
		const time_point<system_clock> lastActivity = it->second.getLastActivity();
		
		Client &client = _clients.at(clientFd);
		Http &http = client.getHttpClass();

		if (checkTimeOut(lastActivity, DEFAULT_TIMEOUT_S))
		{
			if (client.getRecvStatus() != COMPLETE) //still receiving but gave timeout
			{
				_setTimeoutResponse(http, HTTP_REQUEST_TIMEOUT);
				_printDebug("[TIMEOUT]", client, "client inactive for too long", true);
				_closeClientFd(clientFd);
			}
			else if (http.getState() == READY_TO_SEND && !client.isResponseComplete()) //Sending response stuck
			{
				_printDebug("[TIMEOUT]", client, "sending response stuck for too long", true);
				_closeClientFd(clientFd);
			}
			else
			{
				_printDebug("[TIMEOUT]", client, "client inactive for too long", true);
				_closeClientFd(clientFd);
			}
		}
		it++;
	}
}

void Server::_checkCgiTimeouts()
{
	for (size_t i = 0; i < _pollFds.size(); i++) 
	{
		const int fd = _pollFds.at(i).fd;
		if (_isCgiFd(fd)) {
			auto cgi_map = getActiveCgis();
			checkCgiTimeout(cgi_map[fd]);
		}
	}
}

void Server::_checkTimeouts()
{
	_checkClientTimeouts();
	_checkCgiTimeouts();

}

/*
->> during recv, if it takes too long
408 -> we did not receive data in full due to network issues ->timeout

CLIENT TIMEOUTS
├── Receiving request stuck → 408
├── Sending response stuck   → close
└── Idle connection          → close -> no recv no send

CGI TIMEOUT
└── CGI execution stuck      → 504

*/