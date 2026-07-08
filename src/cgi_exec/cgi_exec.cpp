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
#include "configParser.hpp"
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <string.h>
#include <limits.h>
#include <vector>
#include <sstream>

#define PYTHON_EXEC "python"
#define PATH_TO_SCRIPT "/home/avaliull/Projects/lvl5/webserv/server/cgi-bin/hello_world.py"

static void	cgi_bzero(
	char* path,
	size_t size
) {
	for (size_t i = 0; i < size; i++) {
		path[i] = 0;
	}
}

static int	findAndExecuteScript(
	const std::string binary_name,
	char *const argv[],
	std::vector<std::string> paths
)
{
	std::string 	slash_arg;
	char			path[PATH_MAX];	
	extern char**	environ;

	cgi_bzero(path, PATH_MAX);
	for (size_t i = 0; i < paths.size(); i++) {
		slash_arg = paths.at(i) + "/" + binary_name;
		for (size_t j = 0; j < slash_arg.size(); j++) {
			path[j] = slash_arg.at(j);
		}
		execve(path, argv, environ);
		slash_arg.clear();
		cgi_bzero(path, PATH_MAX);
	}
	return (1);
}

static std::vector<std::string>	splitPathVar(
	void
) {
	std::vector<std::string>	paths;
	std::string					path_var = getenv("PATH");

	if (path_var.size() == 0) {
		return (paths);
	}
	std::istringstream	path_stream(path_var);
	std::string			path_part;
	while (std::getline(path_stream, path_part, ':')) {
		paths.push_back(path_part);
		path_part.clear();
	}
	return (paths);
}

static int	tryExecveScript(
	const std::string binary_name,
	char *const argv[]
)
{
	int							err_check;
	std::vector<std::string>	path;

	path = splitPathVar();
	if (path.size() == 0) // add error message about missing path
		return (1);
	err_check = findAndExecuteScript(binary_name, argv, path);
	return (err_check);
}

char**	constructEnvironment(const cfg_server_t& server_config) {
	const t_envVarCGI	env_var_names;
	char**				env;

	char*	new_env_var;
	new_env_var = cgi_prefix_document_root + config.
}

int	executeCGI(
	const cfg_server_t& server_config
) {
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
		std::cout << "executing cgi in child...\n\n";
		tryExecveScript(PYTHON_EXEC, argv); // add checks for fail to exit out of child
		std::cout << "if you see this, there's an error\n";
		exit(1);
	}
	else if (fork_ret > 0) {
		wait(NULL); // lol
	}
	return (execve_ret);
}

