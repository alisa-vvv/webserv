/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 15:58:28 by tutku             #+#    #+#             */
/*   Updated: 2026/08/26 16:16:02 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Listener.hpp"
#include "configParser.hpp"
#include "Http.hpp"
#include "cgi_exec.hpp"


#define SUCCESS 0
#define ERROR -1
#define POLL_TIMEOUT_MS 1000

extern volatile sig_atomic_t	gStop;

enum eServerError
{
	SERVER_OK = 0,
	SERVER_CONFIG_ERR,
	SERVER_LISTENER_SETUP_ERR,
	SERVER_LISTENER_NOT_FOUND_ERR,
	SERVER_POLL_ERR,
	SERVER_ACCEPT_ERR,
	SERVER_SETNONBLOCKING_ERR,
	SERVER_RECV_ERR,
	SERVER_CLIENT_CLOSED,
	SERVER_TIMEOUT_ERR,
	SERVER_SEND_ERR,
	SIGACTION_ERR,
	SERVER_CGI_ERR
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
	std::map<int, cgi_t>		_backgroundCgis;
	std::map<int, int>			_cgiFdToClientFd;

	void						_buildListener(void);
	void						_addFdToPoll(int fd);
	void						_addListenerFdsToPoll();
	
	bool						_isListenerFd(int fd) const;
	bool						_isClientFd(int fd) const;
	bool						_isCgiFd(int fd);

	const Listener*				_findListenerByFd(int fd);
	
	Server(const Server &other);
	Server						&operator=(const Server &other);
	eServerError				_initPollEvent();
	
	eServerError				_pollEvents();
	eServerError				_handleListenerEvent(int i);
	eClientEventResult			_handleClientEvent(int i);
	eServerError				_startCgi(int clientFd);
	eServerError				_handleRecv(int fd);
	eServerError				_checkRecvBuffer(Client &client);
	eServerError				_handleSend(Client& client);
	void						_setPollout(int clientFd);

	eServerError				_acceptClients(int serverListenFd);
	eServerError				_setNonBlocking(int fd);

	//timeout handling
	void						_setTimeoutResponse(Http& http, int timeoutCode);
	void						_checkTimeouts();
	void						_checkClientTimeouts();
	void						_checkCgiTimeouts();

	//closing/removing
	void						_closeClientFd(int fd);
	void						_closeClients();
	void						_closeAll();
	void						_removeFdFromPoll(int fd);
	void						_cleanupCgiForClient(int clientFd);
	
public:
	Server(const Config &config);
	~Server();
	
	eServerError				setup(void);
	eServerError				run();
	int							matchConfig(uint32_t ip, int port, const cfg_server_t &serverConfig);
	
	void						closeListeners();
	
	const std::map<int, cgi_t>&	getActiveCgis() const;
	std::map<int, cgi_t>& 		getActiveCgis();
	void						removeActiveCgi(int cgiFd);
	eClientEventResult			handleCgiEvent(int fd, int i);
	void						copyCgiResponse(int cgiFd, int clientFd);

	//debug
	void						printDebug(const std::string &eventStr, const std::string &infoMsg, bool error); //poll
	void						printDebug(const std::string &eventStr, const Client &client, const std::string &infoMsg, bool error);
	void						printDebug(const std::string &eventStr, const Client &client, int cgiFd, const std::string &infoMsg, bool error); //cgi
	void						printDebug(const std::string &eventStr, int fd, const std::string &infoMsg, bool error = false);
	void						printSection(const std::string &msg);
};

int								setupSignal();

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
