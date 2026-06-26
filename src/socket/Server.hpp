/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:28 by tutku             #+#    #+#             */
/*   Updated: 2026/06/26 15:42:34 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Listener.hpp"
#include "ConfigParser.hpp"

#define SUCCESS 0
#define ERROR -1

extern volatile sig_atomic_t	gStop;

enum eServerError
{
	SERVER_OK  = 0,
	SERVER_CONFIG_ERR,
	SERVER_LISTENER_SETUP_ERR,
	SERVER_POLL_ERR
};

class Server
{
private:
	const Config				&_config;
	std::vector<Listener>		_listeners;
	std::map<int, Client>		_clients;	// client state, found by client fd
	std::vector<struct pollfd> _pollFds;	// the list poll() watches

	void			_buildListener(void);
	void			_closeClientFd(int fd); //TODO: move inside Listener
	Server			&operator=(const Server &other); //TODO: decide what to do, prevent leaks too
	Server(const Server &other);

public:
	Server(const Server &other);
	Server &operator=(const Server &other);
	Server(const Config &config);
	~Server();

	eServerError	setup(void);
	eServerError	run(int i);
	eServerError	_initPollEvent(int i);
	void			_addFdToPoll(int fd);
	eServerError	_accept(int serverListenFd);
	int				get_fd() const;
	void			closeListeners();
	
	//test
	void			printPollInfo(int i);
	
};

int					setupSignal();


#endif

/*
_fd
  the listening/server socket

clientFd
  one connected client socket, returned by accept()

_pollFds vector -> who should I watch?, tells who is ready
  list of all fds poll() should watch
  pollfds has both:
	server fd  -> used for accept()
	client fds -> used for recv()/send()

_clients map -> what do I know about this client?
  actual per-client data/state, looked up by fd
*/

/*
server fd  + POLLIN = accept()
		a new client is waiting to connect

client fd  + POLLIN = recv()
		this connected client sent data, so recv() can read it
*/
