/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   config_parser_main.cpp                                  :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/05/28 14:18:31 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/03 15:58:38 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "configParser.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int	main(void) {
	std::optional<Config>	parse_config_ret = parseConfig(NULL);
	if (parse_config_ret == std::nullopt) {
		return (1);
	}
	Config config = *parse_config_ret;
}
