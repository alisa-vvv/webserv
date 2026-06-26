/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:35 by tutku             #+#    #+#             */
/*   Updated: 2026/06/25 17:21:49 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//std::vector<Listener>	_listener;
void Server::_buildListener()
{
	Listener listenerTemp;

	for (size_t i = 0; i < _config.servers.size(); i++)
	{
		listenerTemp.setIpAddr(_config.servers.at(i).ip_addr);
		for (size_t j = 0; j < _config.servers[i].ports.size(); j++)
		{
			listenerTemp.setPort(_config.servers.at(i).ports[j]);
			_listener.push_back(listenerTemp);
		}
	}
}

eServerError Server::setup(void)
{
	_buildListener();
}

eServerError Server::run(void)
{
	eServerError err;

	err = this->_initPollEvent();
	if (err != SERVER_OK)
	{
		closeListeners();
		return SERVER_POLL_ERR;
	}
	return SERVER_OK;
}

// Add fd to the list of fds that poll() should watch
void Server::_addFdToPoll(int fd)
{
	struct pollfd pollFdServer;

	pollFdServer.fd = fd;
	pollFdServer.events = POLLIN;
	pollFdServer.revents = 0;

	_pollFds.push_back(pollFdServer);
}

/*
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
poll() tells you which fd is ready to do something.
poll() only fills revents inside existing _pollFds

fd      = the fd you want to watch
events  = what you are interested in
revents = what actually happened
if fd < 0 --> revents return 0

return values:
> 0   number of fds with events
0   timeout happened
-1   error, wait forever until something happens

POLLIN There is data to read -> a new client is trying to connect
// A POLLHUP means the socket is no longer connected
//		timeout values:
// poll(fds, count, -1);    // Wait forever
// poll(fds, count, 0);     // Do not wait
// poll(fds, count, 1000);  // Wait up to one second
*/
eServerError Server::_initPollEvent()
{
	int pollFdCount;

	_addFdToPoll(_fd);
	while (gStop == 0)
	{
		pollFdCount = poll(_pollFds.data(), _pollFds.size(), 1000);
		//_pollFds.push_back(pollfd_for_clientFd);
		//_clients[clientFd] = Client(clientFd);
		if (pollFdCount == ERROR)
		{
			if (errno == EINTR) // when a system call in interrupted by an incoming signal
			{
				if (gStop == 1)
					break;
				continue; // try again
			}
			return SERVER_POLL_ERR;
		}
		if (pollFdCount == 0)
		{
			continue;
		}
		if (pollFdCount > 0)
		{
			//TODO: create a pollEvent function and move the for loop there

		}
		for (int i = 0; i < (int)(_pollFds.size()); i++)
		{
			if (_pollFds[i].revents == 0)
				continue;
			if (_pollFds[i].revents != 0)
			{
				printPollInfo(i); //test
				
				if (_pollFds[i].fd == _fd) //new client waiting to connect //TODO: change later, now it only works for one listening socket
				{
					if ((_pollFds[i].revents & POLLIN))
					{
						_accept(_pollFds[i].fd);
					}
					//TODO:handle listening-socket errors here
				}
				else 
				{
					if (_pollFds[i].revents & POLLIN)
					{
						// client fd is ready: recv data
						printf("TODO: recv comes here");
					}
					else if (_pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
					{
						_closeClientFd(_pollFds[i].fd);
						i--;
						continue;//TODO:remove if nothing is added after it
					}
				}
			}
			//_checkTimeouts(); //TODO
		}
		// check all fds
	}
	// TODO: add return
}

/*
remove clientFd from _pollFds
erase clientFd from _clients
*/
void Server::_closeClientFd(int fd)
{

	for (int i = 0; i < (int)(_pollFds.size()); i++)
	{
		if (fd == _pollFds[i].fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}
	_clients.erase(fd);
	close(fd);
}

Server::Server() : _config(config),_fd(-1)
{
	_port = 8080; //TODO: remove when config is ready
	_host = INADDR_ANY; //TODO: remove when config is ready
	memset(&_address, 0, sizeof(_address)); //TODO: move when config is ready
}

Server::~Server()
{
	closeListeners();
}

// Server &Server::operator= (const Server &other)
// {
// 	if (this != &other)
// 	{
// 		this->_fd = other._fd;
// 		this->_address = other._address;
// 	}
// 	return (*this);
// }

// Server::Server(const Server &other)
// {
// 	*this = other;
// }
