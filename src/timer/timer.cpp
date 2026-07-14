/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   timer.cpp                                               :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/07/14 12:40:04 by avaliull            #+#    #+#           */
/*   Updated: 2026/07/14 13:00:02 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "Timer.hpp"

bool	checkTimeOut(
	const time_point<system_clock> timer,
	const int time_in_seconds
) {
	using std::chrono::duration_cast;
	using std::chrono::seconds;

	const time_point<system_clock>	cur_time = system_clock::now();
	const seconds					execution_time = duration_cast<seconds>(cur_time - timer);

	return (execution_time.count() > time_in_seconds);
}
