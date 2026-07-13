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


	/*	These are things we're most likely not going to have. Putting them here in case */
//"HTTP_COOKIE=", we don't have cookies
//"HTTP_HOST=", // all these http related ones seem to be out of scope for us
//"HTTP_REFERER=",
//"HTTP_USER_AGENT=",
//"HTTPS=",
//"REMOTE_USER=", we don't have authentification so we don't need this
//"SERVER_ADMIN=", no auth

	/*	These are a maybe, need to check:	*/
//"REQUEST_URI=", not sure what this is, we might need it?
//"SCRIPT_FILENAME=", same as above

static char**	constructEnvironment(const cfg_server_t& server_config) {
	std::vector<std::string>	vars = {
		"DOCUMENT_ROOT=" + server_config.root, // root directory of the server
		"PATH=" + (std::string) getenv("PATH"),
		"QUERY_STRING=", // if method is GET, here we put the query (where is that from?)
		"REMOTE_ADDR=", // ip of the visitor
		"REMOTE_HOST=", // host of visitor
		"REMOTE_PORT=", // port of visitor
		"REQUEST_METHOD=", // GET or POST
		"SCRIPT_NAME=", // path to the script we're executing
		"SERVER_NAME=" + server_config.server_names[0],
		"SERVER_PORT=", // since we have virtual servers, that one comes from ally's part
		"SERVER_SOFTWARE=webserv",
	};

	char**	env = new char*[vars.size() + 1];
	for (size_t i = 0; i < vars.size(); i++) {
		const std::string&	cur_string = vars.at(i);
		env[i] = new char[cur_string.size() + 1];
		for (size_t j = 0; j < cur_string.size(); j++) {
			env[i][j] = cur_string.at(j);
		}
		env[i][cur_string.size()] = '\0';
	}
	env[vars.size()] = NULL;
	return (env);
}

static int	findAndExecuteScript(
	const cfg_server_t& server_config,
	const std::string binary_name,
	char *const argv[],
	std::vector<std::string> paths
)
{
	std::string 	slash_arg;
	char			path[PATH_MAX];	
	char**			env = constructEnvironment(server_config);

	cgi_bzero(path, PATH_MAX);
	for (size_t i = 0; i < paths.size(); i++) {
		slash_arg = paths.at(i) + "/" + binary_name;
		for (size_t j = 0; j < slash_arg.size(); j++) {
			path[j] = slash_arg.at(j);
		}
		execve(path, argv, env);
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
	const cfg_server_t& server_config,
	const std::string binary_name,
	char *const argv[]
)
{
	int							err_check;
	std::vector<std::string>	path;

	path = splitPathVar();
	if (path.size() == 0) // add error message about missing path mabe
		return (1);
	err_check = findAndExecuteScript(server_config, binary_name, argv, path);
	return (err_check);
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
		tryExecveScript(server_config, PYTHON_EXEC, argv); // add checks for fail to exit out of child
		std::cout << "if you see this, there's an error\n";
		exit(1);
	}
	else if (fork_ret > 0) {
		wait(NULL); // lol
	}
	return (execve_ret);
}
