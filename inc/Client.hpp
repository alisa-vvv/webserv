/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:25 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/08/18 09:52:16 by tcakir-y         ###   ########.fr       */
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
		Http						&getHttpClass();
		const Http					&getHttpClass() const;

		/* ============================== SETTERS ============================== */
		void						setLastActivity(time_point<system_clock> lastActivity);
		void						setRecvStatus(receiveStatus recvStatus);
		void						setResponse(const std::string& response);
		void						setResponseStatus(bool response);
		void						setHttpClass(Http http);

		
		/* ======================== RESPONSE HANDLING ========================= */
		void						updateBytesSent(size_t bytes);
		bool						isResponseComplete() const;

		/* ========================== TIMEOUT HANDLING ========================== */
		void						updateLastActivity();

		/* ========================== HTTP HANDLER ========================== */
		void						clientHandler(const std::string &recvStr);

};


#endif
