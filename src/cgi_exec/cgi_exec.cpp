/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   cgi_exec.cpp                                            :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/25 12:04:11 by avaliull            #+#    #+#           */
/*   Updated: 2026/07/14 13:00:24 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#include "cgi_exec.hpp"
#include "configParser.hpp"
#include "Timer.hpp"
#include <unistd.h>
#include <fcntl.h>
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

static int	gotCGIOutput(
	cgi_t&	cgi,
	int*	p_status
) {
	const bool	timed_out = checkTimeOut(cgi.timer, DEFAULT_TIMEOUT_S_CGI);

	if (timed_out) {
		std::cout << "cgi execution took too long...\n";
		// we throw timeout error
		kill(cgi.child_pid, SIGTERM);
		close(cgi.input);
		close(cgi.output);
		return (1);
	}

	if (int wait_res = waitpid(cgi.child_pid, p_status, WNOHANG) != 0) {
		if (wait_res > 0) {
			char buffer[4096];
			cgi_bzero(buffer, 4096);
			read(cgi.output, buffer, 4096); // mayybe recv with MSG_DONTWAIT
			// should be looped as well since it can be arbitrarily long
			for (int i = 0; buffer[i] != '\0'; i++) {
				cgi.output_string.push_back(buffer[i]);
			}
			std::cout << cgi.output_string;
			close(cgi.input);
			close(cgi.output);
		}
		else if (wait_res < 0) {
			// brr brr error
			return (-1);
		}
		return (1);
	}
	return (0);
}

// returns
// 	int 0: no response from cgi
// 	int 1: got response from cgi
// 	int -1: error when reading cgi response
// 	size_t 0: whenever int is not 1
// 	size_t i: index of the cgi instance that returned a response
std::tuple<int, size_t>	checkBackgroundCGIs(
	std::vector<cgi_t>&	background_cgis
) {
	int			p_status = 0;
	int			got_output;

	for (size_t i = 0; i < background_cgis.size(); i++) {
		do { // REPLACE THIS do while with a single check in actual code.
			got_output = gotCGIOutput(background_cgis.at(i), &p_status);
		} while (got_output == false);
		if (got_output == true)
			return (std::tuple<int, size_t> { 1, i });
		else if (got_output == -1) // error
			return (std::tuple<int, size_t> { -1, 0 });
	}
	return (std::tuple<int, size_t> { 0, 0 });
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
	if (path.size() == 0)
		return (1);
	err_check = findAndExecuteScript(server_config, binary_name, argv, path);
	return (err_check);
}

// 1. epoll() the pipe both in and out
// 2. then send it (the body of the CGI request) -- body from http parser
// 3. when ready, read until EOF or death of child process
// 4. prepare response
static void	handle_child(
	const cfg_server_t& server_config,
	const std::string binary_name,
	char *const argv[],
	int in_pipe[2],
	int out_pipe[2]
) {
	close(in_pipe[1]);
	close(out_pipe[0]);
	dup2(in_pipe[0], STDIN_FILENO); // add error checks on dup2?
	dup2(out_pipe[1], STDOUT_FILENO);
	close(in_pipe[0]);
	close(out_pipe[1]);
	std::cout << "executing cgi in child...\n\n";
	tryExecveScript(server_config, binary_name, argv);
	std::cout << "if you see this, there's an error\n"; // delete this
	exit(1);
}

static cgi_t	handle_parent(
	int in_pipe[2],
	int out_pipe[2],
	int child_pid
) {
	cgi_t	cgi;

	time_point<system_clock>	cgi_timer = system_clock::now();
	close(in_pipe[0]);
	close(out_pipe[1]);
	fcntl(in_pipe[1], O_NONBLOCK | O_CLOEXEC); // need to check the timing on this
	fcntl(out_pipe[0], O_NONBLOCK | O_CLOEXEC); // also error check i guess
	cgi.child_pid = child_pid;
	cgi.input = in_pipe[1];
	cgi.output = out_pipe[0];
	cgi.timer = cgi_timer;
	return (cgi);
}

// two pipes
// parent writes to input pipe and reads from output pipe
// child reads from input pipe and writes to out pipe (dup2 that shit)
std::optional<cgi_t>	executeCGI(
	const cfg_server_t& server_config
) {
	int	in_pipe[2];
	int	out_pipe[2];

	if (pipe2(in_pipe, O_NONBLOCK) != 0) {
		// brr brr errorr
		return (std::nullopt);
	}
	if (pipe2(out_pipe, O_NONBLOCK) != 0) {
		close(in_pipe[0]);
		close(in_pipe[1]);
		// brr brr errorr
		return (std::nullopt);
	}

	std::vector<cgi_t>		background_cgis;
	std::tuple<int, size_t>	cgi_response;
	cgi_t	cgi;
	cgi_t	cgi_response_data;
	char*	argv[] { NULL, NULL, NULL };
	argv[0] = strdup(PYTHON_EXEC);
	argv[1] = strdup(PATH_TO_SCRIPT);
	argv[2] = NULL;

	int	fork_ret = fork();
	if (fork_ret < 0) {
		// brr brr errorr
		return (std::nullopt);
	}
	else if (fork_ret == 0) {
		handle_child(server_config, PYTHON_EXEC, argv, in_pipe, out_pipe);
	}
	else if (fork_ret > 0) {
		cgi = handle_parent(in_pipe, out_pipe, fork_ret);
		background_cgis.push_back(cgi);
	}
	{	// this block is, essentially, what we need to do in the listen loop.
		// the while (1) is the stand-in for the listen loop.
		while (!background_cgis.empty()) {
			cgi_response = checkBackgroundCGIs(background_cgis);
			const auto& [cgi_responded, cgi_index] = cgi_response;
			if (cgi_responded == 1) {
				cgi_response_data = background_cgis.at(cgi_index);
				background_cgis.erase(background_cgis.begin() + cgi_index);
				break ;
			}
			else if (cgi_responded == -1) {
				// brr brr error
				return std::nullopt;
			}
		}
		// at the end of the program, run this for every previously launched cgi
		// cout message unnecessary
		while (waitpid(cgi.child_pid, NULL, WNOHANG) == 0);
		std::cout << "process terminated\n";
	}
	// if response has Status (case insensitive) header, than that's the status
	// otherwise, 200
	// 
	// MAKE THESE REASONABLE ->
	free(argv[0]);
	free(argv[1]);
	return (cgi);
}
