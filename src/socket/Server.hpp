/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:28 by tutku             #+#    #+#             */
/*   Updated: 2026/07/17 01:12:48 by tutku            ###   ########.fr       */
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
	SERVER_OK = 0,
	SERVER_CONFIG_ERR,
	SERVER_LISTENER_SETUP_ERR,
	SERVER_POLL_ERR,
	SERVER_ACCEPT_ERR,
	SERVER_SETNONBLOCKING_ERR,
	SERVER_RECV_ERR,
	SERVER_CLIENT_CLOSED,
	SERVER_TIMEOUT_ERR
};

enum eClientEventResult
{
	CLIENT_KEPT,
	CLIENT_REMOVED
};

class Server
{
private:
	const Config				&_config;
	std::vector<Listener>		_listeners;
	std::map<int, Client>		_clients;	// client state, found by client fd
	std::vector<struct pollfd>	_pollFds;	// the list poll() watches

	void				_buildListener(void);
	void				_addFdToPoll(int fd);
	void				_addListenerFdsToPoll();
	
	int					_isListenerFd(int fd) const;
	
	Server(const Server &other);
	Server				&operator=(const Server &other);
	eServerError		_initPollEvent();
	
	eServerError		_pollEvents();
	eServerError		_handleListenerEvent(int i);
	eClientEventResult	_handleClientEvent(int i);
	eServerError		_handleRecv(int fd);
	eServerError		_handleSend(int fd);

	eServerError		_acceptClients(int serverListenFd);
	eServerError		_setNonBlocking(int fd);
	void				_checkTimeouts(); //TODO:finish

	void				_closeClientFd(int fd);
	void				_closeClients();
	void				_closeAll();
	
	public:
	Server(const Config &config);
	~Server();
	
	eServerError	setup(void);
	eServerError	run();
	int				matchConfig(uint32_t ip, int port, const cfg_server_t &serverConfig);
	
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
