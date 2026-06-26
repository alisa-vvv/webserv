/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 14:04:20 by tutku             #+#    #+#             */
/*   Updated: 2026/06/25 17:21:22 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
Use listening/server fd to accept one waiting connection.
It returns a new client fd.
*/
eServerError Server::_accept(int serverListenFd)
{
	while (1)
	{
		int clientFd = accept(serverListenFd, NULL, NULL);

		if (clientFd >= 0)
		{
			if (_setNonBlocking(clientFd) != SERVER_OK)
			{
				close(clientFd);
				continue;
			}
			_addFdToPoll(clientFd);

			Client newClient(clientFd);
			_clients[clientFd] = newClient;
			continue;
		}
		else if (errno == EWOULDBLOCK || errno == EAGAIN)
			break;
		if (errno == EINTR || errno == ECONNABORTED) // check accept() man
			continue;
		std::cerr << "Error accept(): " << std::strerror(errno) << std::endl;
		return SERVER_ACCEPT_ERR;
	}
	return SERVER_OK;
}

void Server::closeListeners()
{
	for (int i = 0; i < _listener.size(); i++)
	{
		if (_listener[i].getListenerFd() != -1)
		{
			close(_listener[i].getListenerFd());
			_listener[i].setListenerFd(-1);
		}
	}
}

/*
instead of “one server fd + many client fds”,
 poll list becomes “many listening/server fds + many client fds”
 The logic changes from this:

_pollFds:
[ server fd 8080 ]
[ client fd ]
[ client fd ]

to this:

_pollFds:
[ server/listening fd 8080 ]
[ server/listening fd 9090 ]
[ server/listening fd 3000 ]
[ client fd from 8080 ]
[ client fd from 9090 ]
 Every port needs its own listening socket fd, 
 and all of those listening fds go into the same poll() vector.
 std::map<int, s_listener> _listeners;
std::vector<pollfd> _pollFds;
std::map<int, Client> _clients;

in setup
{
for each port/host in config
	create socket
	....
	listen

	save listener in listeners list
}
*/