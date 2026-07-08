/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RcvBuffer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 12:57:08 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/08 13:22:27 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <time.h>
#define MAX_REQUEST 8192
#define BUFFER_MAX 4096
#define TIME_OUT_RECV 1000 //make a timeout function that checks the activity

enum receiveStatus{
	SOCKET_CLOSED,
	RECV_ERROR,
	COMPLETE,
	INCOMPLETE,
	MAXBYTESRECEIVED,
	TIMEOUT
};


class RcvBuffer {
	private:
		std::string				recvStr;

	public:
		RcvBuffer();
		ssize_t					totalBytesReceived;
		receiveStatus			checkStatus();
		void					append(char *buffer, ssize_t size);
		const char*				getRecvStr() const { return this->recvStr.c_str(); }
};