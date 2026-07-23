/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:25 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/21 16:19:42 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <map>
#include <vector>
#include "RcvBuffer.hpp"
#include "Http.hpp"
#include <chrono>
using std::chrono::time_point;
using std::chrono::system_clock;

class Http;
class Listener;


class Client
{
	private:
		int							_listenerFd;
		int							_clientFd;
		RcvBuffer					_rcvBuffer;
		time_point<system_clock>	_lastActivity;
		const Listener				*_listener;

		receiveStatus				_recvStatus;
		std::string					_response;
		bool						_responseStatus;
		size_t						_bytesSent;
		// clientState					_clientState; //COMMENTED OUT BY ALLY, REASON: CLASH WITH HTTP
		Http						_http;

	public:
		/* ============================ CONSTRUCTORS ============================ */
		Client();
		Client(const Listener *listener, int clientFd);
		~Client();

		/* ============================== GETTERS ============================== */
		time_point<system_clock>	getLastActivity() const;
		receiveStatus				getRecvStatus() const;
		const std::string			&getResponse() const;
		bool						getResponseStatus() const;
		size_t						getBytesSent() const;
		int							getListenerFd() const;
		RcvBuffer					&getRcvBuffer();
		const Listener*				getListenerClass() const;
		int 						getClientFd() const;
		// clientState					getClientState() const; //COMMENTED OUT BY ALLY, REASON: CLASH WITH HTTP
		Http						&getHttpClass();

		/* ============================== SETTERS ============================== */
		void						setLastActivity(time_point<system_clock> lastActivity);
		void						setRecvStatus(receiveStatus recvStatus);
		void						setResponse(const std::string& response);
		void						setResponseStatus(bool response);
		// void						setClientState(clientState clientState); //COMMENTED OUT BY ALLY, REASON: CLASH WITH HTTP
		void						setHttpClass(Http http);

		
		/* ======================== RESPONSE HANDLING ========================= */
		void						updateBytesSent(size_t bytes);
		bool						isResponseComplete() const;

		/* ========================== TIMEOUT HANDLING ========================== */
		void						updateLastActivity();

		/* ========================== HTTP HANDLER ========================== */
		void						clientHandler(std::string recvStr);

};


#endif
