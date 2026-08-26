/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_exec.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 13:00:51 by avaliull          #+#    #+#             */
/*   Updated: 2026/08/26 13:58:53 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <optional>
#include "Client.hpp"

class	cgi_t {
public:
	cgi_t() = delete;
	cgi_t(Client&);

	int							child_pid = -1;
	int							input = -1;
	int							output = -1;
	std::string					input_string = "";
	std::string					output_string = "";
	time_point<system_clock>	timer;
	bool						timed_out = false;
	Client&						client;
};

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
void	killCgi(cgi_t& cgi);

std::string buildCGIResponseString(std::string cgiResponse);

void	checkCgiTimeout(cgi_t& cgi);
