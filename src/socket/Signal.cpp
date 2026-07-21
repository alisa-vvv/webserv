/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 00:04:59 by tutku             #+#    #+#             */
/*   Updated: 2026/07/20 01:14:09 by tutku            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

volatile sig_atomic_t gStop = 0;

static void handler(int signum)
{
	(void)signum;
	gStop = 1;
}

int setupSignal()
{
	struct sigaction sa;

	std::memset(&sa, 0, sizeof(sa));
	
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		std::cerr << "Error sigaction failed" << std::endl;
		return SIGACTION_ERR;
	}
	if (sigaction(SIGTERM, &sa, NULL) == -1)
	{
		std::cerr << "Error sigaction failed" << std::endl;
		return SIGACTION_ERR;
	}

	// server tries to send() to a client that already disconnected.
	// Ignoring it prevents the whole server from dying 
	// because one client disappeared.
	struct sigaction sigpipe;

	std::memset(&sigpipe, 0, sizeof(sigpipe));
	sigemptyset(&sigpipe.sa_mask);
	sigpipe.sa_flags = 0;
	sigpipe.sa_handler = SIG_IGN;
	
	if (sigaction(SIGPIPE, &sigpipe, NULL) == -1)
	{
		std::cerr << "Error: sigaction failed for SIGPIPE" << std::endl;
		return SIGACTION_ERR;
	}
	return SERVER_OK;
}
