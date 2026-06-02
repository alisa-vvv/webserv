/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   config_parser_main.cpp                                  :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 14:18:31 by avaliull            #+#    #+#           */
/*   Updated: 2026/05/28 15:28:12 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int	exampleUsage(const Config& config) {
	const char*			host_str = config.servers[0].listen.host;
	const char* 		port = config.servers[0].listen.port;
	struct addrinfo*	addrinfo_holder;

	int status = getaddrinfo(host_str, port, NULL, &addrinfo_holder);
	return (status);
}

int	main(void) {
	parseConfig();
	Config	newConf;

	std::cout << newConf.servers[0].listen.host;
}
