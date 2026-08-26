/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   Timer.hpp                                               :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/07/14 12:40:17 by avaliull            #+#    #+#           */
/*   Updated: 2026/08/25 17:20:30 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <chrono>

#define DEFAULT_TIMEOUT_S 30
#define DEFAULT_TIMEOUT_S_CGI 30

using std::chrono::time_point;
using std::chrono::system_clock;

bool	checkTimeOut(const time_point<system_clock> timer, const int time);
time_point<system_clock>	setTimer();
