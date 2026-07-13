/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   cgi_exec.hpp                                            :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/25 13:00:51 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/25 13:02:57 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <optional>
#include <chrono>
#include "configParser.hpp"

#define DEFAULT_TIMEOUT_S 30

using std::chrono::time_point;
using std::chrono::system_clock;
typedef struct	cgi_t {
	int							child_pid = -1;
	int							input = -1;
	int							output = -1;
	std::string					input_string = "";
	std::string					output_string = "";
	time_point<system_clock>	timer;
}	cgi_t;

std::optional<cgi_t>	executeCGI(
	const cfg_server_t& server_config
);
