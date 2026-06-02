/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   config_parser_main.cpp                                  :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 14:18:31 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/02 18:17:42 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include "ParsingInfo.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int	main(void) {
	const	ParsingInfo	parsing_info;
	parseConfig();
	Config	newConf;

	std::cout << newConf.servers[0].listen.host;
}
