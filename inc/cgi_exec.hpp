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

std::optional<cgi_t>	executeCGI(
	const cfg_server_t& server_config
);
std::tuple<int, size_t>	checkBackgroundCGIs(
	std::vector<cgi_t>&	background_cgis
);
