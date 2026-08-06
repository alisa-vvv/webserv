/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   cgi_exec.hpp                                            :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/25 13:00:51 by avaliull            #+#    #+#           */
/*   Updated: 2026/07/14 12:47:59 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <optional>
#include "Http.hpp"
#include "Client.hpp"

// might mvoe this declaration elsewhere, still confused about our scturcuture
// i think if it's not declared here we're making a big mistake lol
typedef struct	cgi_t {
	int							child_pid = -1;
	int							input = -1;
	int							output = -1;
	std::string					input_string = "";
	std::string					output_string = "";
	time_point<system_clock>	timer;
	Client						client;
}	cgi_t;

std::optional<cgi_t>	executeCGI(
	Client& client,
	std::map<int, cgi_t>&	bacgkround_cgis
);
std::tuple<int, size_t>	checkBackgroundCGIs(
	const Client& client,
	std::map<int, cgi_t>&	background_cgis
);
int	checkCgiDone(
	cgi_t&	cgi
);

