/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 15:41:25 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/14 16:24:19 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <map>
#include <vector>
#include "RcvBuffer.hpp"
#include "ConfigParser.hpp"

/*
client obj answers:

	What data does this client have?
	What state is this client in?
	Should I read from it?
	Should I write to it?
	Should I close it?
*/
class Client
{
	private:
		int							_listenerFd;
		int							_clientFd;
		RcvBuffer					_rcvBuffer;
		std::vector<cfg_server_t *>	_serverConfigs;
		time_t						_lastActivity;

	public:
		Client();
		Client(int listenerFd, int clientFd);
		~Client();

		time_t		getLastActivity();
		int			getListenerFd();
		RcvBuffer&	getRcvBuffer();

		void		setLastActivity(time_t lastActivity);

		void		updateLastActivity();
};

#endif
