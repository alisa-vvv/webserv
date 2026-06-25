/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   cgi_exec.cpp                                            :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/25 12:04:11 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/25 13:40:40 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "cgi_exec.hpp"
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <string.h>

#define PYTHON_EXEC "python"
#define PATH_TO_SCRIPT "/home/avaliull/Projects/lvl5/webserv/server/cgi-bin/hello_world.py"

/// WE NEED STRDUP
int	executeCGI() {
	extern char		**environ;
	char*	argv[] { NULL, NULL, NULL };
	argv[0] = strdup(PYTHON_EXEC);
	argv[1] = strdup(PATH_TO_SCRIPT);
	argv[2] = NULL;
	int			execve_ret = 0;

	int	fork_ret = fork();
	if (fork_ret < 0) {
		// brr brr errorr
		return (1);
	}
	else if (fork_ret == 0) {
		std::cout << "executing cgi in child...\n";
		execve_ret = execve("python", argv, environ);
		std::cout << execve_ret;
	}
	else if (fork_ret > 0) {
		wait(NULL);
	}
	return (execve_ret);
}
