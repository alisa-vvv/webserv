/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_exec.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 12:04:11 by avaliull          #+#    #+#             */
/*   Updated: 2026/08/26 14:14:14 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi_exec.hpp"
#include "utils.hpp"
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

cgi_t::cgi_t(Client&	client_ref)
	:	client(client_ref)
{}

void	fill_c_str_from_string(char *const c_str, const std::string& str) {
	for (size_t i = 0; i < str.size(); i++) {
		c_str[i] = str.at(i);
	}
	c_str[str.size()] = '\0';
}

std::string buildCGIResponseString(std::string cgiResponse)
{
	size_t firstSpace = cgiResponse.find(" ");
	size_t newLine = cgiResponse.find("\r\n"); 
	std::string startLine = cgiResponse.substr(0, firstSpace);
	std::string cgiResponseString;
	if (startLine == "HTTP/1.1" || startLine == "HTTP/1.0")
		cgiResponseString = cgiResponse;
	else
	{
		cgiResponseString += "HTTP/1.1 200 OK\r\n";
		if (newLine != std::string::npos) {
			cgiResponseString += cgiResponse.substr(newLine + 2);
		}
		else
			cgiResponseString += cgiResponse;
	}
	return cgiResponseString;
}

static void	cgi_bzero(
	char* path,
	size_t size
) {
	for (size_t i = 0; i < size; i++) {
		path[i] = 0;
	}
}

void	killCgi(cgi_t& cgi) {
	cgi.timed_out = true;
	std::cout << CLR_YEL << "[CGI TIMEOUT] " << CLR_NON;
	std::cout << "killing cgi process (fd: " << cgi.output << ") due to timeout\n";
	kill(cgi.child_pid, SIGTERM);
}

void	checkCgiTimeout(cgi_t& cgi) {
	if (cgi.timed_out == true)
		return ;
	const bool	timed_out = checkTimeOut(cgi.timer, DEFAULT_TIMEOUT_S_CGI);

	if (timed_out) {
		killCgi(cgi);
	}
}

int	gotCGIOutput(
	cgi_t&	cgi
) {
	checkCgiTimeout(cgi);

	int			p_status = 0;
	static const int cgi_recv_buf = 512;
	int wait_res = waitpid(cgi.child_pid, &p_status, WNOHANG);
	if (wait_res > 0) {
		char buffer[cgi_recv_buf];
		int	recv_ret;
		do {
			cgi_bzero(buffer, cgi_recv_buf);
			recv_ret = read(cgi.output, buffer, cgi_recv_buf - 1);
			for (int i = 0; buffer[i] != '\0'; i++) {
				cgi.output_string.push_back(buffer[i]);
			}
		} while (recv_ret > 0);
		if (cgi.input > 0)
			close(cgi.input);
		cgi.input = -1;
		if (cgi.output > 0)
			close(cgi.output);
		cgi.output = -1;
		return (true);
	}
	else if (wait_res < 0) {
		// brr brr error
		return (-1);
	}
	return (false);
}

// returns
// 	int 0: no response from cgi
// 	int 1: got response from cgi
// 	int -1: error when reading cgi response
// 	size_t 0: whenever int is not 1
// 	size_t i: index of the cgi instance that returned a response
int	checkCgiDone(
	cgi_t&	cgi
) {
	int			got_output;

	got_output = gotCGIOutput(cgi);
	if (got_output == -1) {
		return (-1);
	}
	if (got_output == true) {
		Http& http = cgi.client.getHttpClass();
		if (cgi.timed_out) {
			createErrorResponse(cgi.client, cgi.client.getHttpClass());
		}
		else {
			std::string	response_string = buildCGIResponseString(cgi.output_string);
			cgi.client.setResponse(response_string);
		}
		http.setState(READY_TO_SEND);
		return (true);
	}
	return (0);
}

static const std::string match_method_to_string(
	httpMethod	method
) {
	switch (method) {
		case GET:
			return ("GET");
		case POST:
			return ("POST");
		case DELETE:
			return ("DELETE");
		case UNKNOWN:
		default:
			return ("UNKNOWN");
			
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

static char**	constructEnvironment(
	Client& client
) {
	const Http&	request_data = client.getHttpClass();
	const cfg_server_t&	server_config = *request_data.requestConfig.server;
	const t_location&	location = *request_data.requestConfig.location;
	std::string	request_uri = request_data.getReceivedUri();
	std::string	query_string;
	size_t		query_string_start = query_string.find_first_of('?') + 1;
	if (query_string_start != std::string::npos) {
		query_string = request_uri.substr(query_string_start + 1, request_uri.back());
	}
	std::vector<std::string>	vars = {
		"DOCUMENT_ROOT=" + server_config.root, // root directory of the server
		"PATH=" + (std::string) (getenv("PATH")), // CHECK THIS
		"PWD=" + (std::string) (getenv("PWD")),
		"QUERY_STRING=" + query_string,
		"REQUEST_METHOD=" + match_method_to_string(request_data.getMethod()), // GET or POST
		"REQUEST_URI=" + request_data.getReceivedUri(),
		// FIX BELOW!!!
		"SCRIPT_FILENAME=" + location.cgi_pass.path,  // path to the script (absolute)
		"SCRIPT_NAME=" + location.cgi_pass.path, // path to the script we're executing relative to root
		"SERVER_NAME=" + server_config.server_names[0],
		"SERVER_PORT=" + std::to_string(server_config.ports.at(0)),
		"SERVER_SOFTWARE=webserv",
	};

	char**	env = new char*[vars.size() + 1];
	for (size_t i = 0; i < vars.size(); i++) {
		//std::cerr << "path var: " << vars.at(i) << '\n';
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
	Client& client,
	const std::string binary_name,
	char *const argv[],
	std::vector<std::string> paths
) {
	std::string 	slash_arg;
	char			path[PATH_MAX];	
	char**			env = constructEnvironment(client);

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
	Client& client,
	const std::string binary_name,
	char *const argv[]
)
{
	int							err_check;
	std::vector<std::string>	path;

	path = splitPathVar();
	if (path.size() == 0)
		return (1);
	err_check = findAndExecuteScript(client, binary_name, argv, path);
	return (err_check);
}

static void	handle_child(
	Client& client,
	const std::string binary_name,
	char *const argv[],
	int in_pipe[2],
	int out_pipe[2]
) {
	close(in_pipe[1]);
	close(out_pipe[0]);
	dup2(in_pipe[0], STDIN_FILENO);
	dup2(out_pipe[1], STDOUT_FILENO);
	close(in_pipe[0]);
	close(out_pipe[1]);
	tryExecveScript(client, binary_name, argv);
	exit(1);
}

static void	handle_parent(
	cgi_t&	new_cgi,
	int in_pipe[2],
	int out_pipe[2],
	int child_pid
) {
	time_point<system_clock>	cgi_timer = system_clock::now();
	close(in_pipe[0]);
	close(out_pipe[1]);
	fcntl(in_pipe[1], O_NONBLOCK | O_CLOEXEC);
	fcntl(out_pipe[0], O_NONBLOCK | O_CLOEXEC);
	new_cgi.child_pid = child_pid;
	new_cgi.input = in_pipe[1];
	new_cgi.output = out_pipe[0];
	new_cgi.timer = cgi_timer;
}

std::optional<cgi_t>	executeCGI(
	Client& client,
	std::map<int, cgi_t>&	background_cgis
) {
	AllowedCgi	allowed_cgi;
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

	cgi_t	cgi(client);
	const e_cgi_extension type = client.getHttpClass().requestConfig.location->cgi_pass.extension;

	char*	argv[] { NULL, NULL, NULL };
	argv[0] = new char[allowed_cgi.executables.at(type).size() + 1];
	fill_c_str_from_string(&argv[0][0], allowed_cgi.executables.at(type));
	argv[1] = new char[(client.getHttpClass().getBuiltUri()).size() + 1];
	fill_c_str_from_string(&argv[1][0], client.getHttpClass().getBuiltUri());
	argv[2] = NULL;

	int	fork_ret = fork();
	if (fork_ret < 0) {
		// brr brr errorr
		return (std::nullopt);
	}
	else if (fork_ret == 0) {
		handle_child(client,
			   PYTHON_EXEC, argv, in_pipe, out_pipe);
	}
	else if (fork_ret > 0) {
		handle_parent(cgi, in_pipe, out_pipe, fork_ret);
		cgi.client = client;
		background_cgis.insert( {cgi.output, cgi} );
	}
	delete[] argv[0];
	delete[] argv[1];
	return (cgi);
}
