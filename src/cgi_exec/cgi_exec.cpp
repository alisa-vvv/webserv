/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_exec.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 12:04:11 by avaliull          #+#    #+#             */
/*   Updated: 2026/08/18 10:31:44 by tcakir-y         ###   ########.fr       */
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
// root of location + cgi_pass

// t his maybe remove? if not, remove from http class
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

//static std::string buildCGIResponseString(std::string cgiResponse)
//{
//	size_t firstSpace = cgiResponse.find(" ");
//	size_t newLine = cgiResponse.find("\r\n"); 
//	std::string startLine = cgiResponse.substr(0, firstSpace);
//	std::string cgiResponseString;
//	if (startLine != "HTTP/1.1" || startLine != "HTTP/1.0")
//		cgiResponseString = cgiResponse;
//	else
//	{
//		cgiResponseString += "HTTP/1.1 200 OK\r\n";
//		cgiResponseString += cgiResponse.substr(newLine + 2);
//	}
//	return cgiResponseString;
//}

static void	cgi_bzero(
	char* path,
	size_t size
) {
	for (size_t i = 0; i < size; i++) {
		path[i] = 0;
	}
}

int	gotCGIOutput(
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
		return (-1);
	}

	#define CGI_RECV_BUF 512
	if (int wait_res = waitpid(cgi.child_pid, p_status, WNOHANG) != 0) {
		if (wait_res > 0) {
			char buffer[CGI_RECV_BUF];
			int	recv_ret;
			do {
				cgi_bzero(buffer, CGI_RECV_BUF);
				recv_ret = read(cgi.output, buffer, CGI_RECV_BUF);
				for (int i = 0; buffer[i] != '\0'; i++) {
					cgi.output_string.push_back(buffer[i]);
				}
			} while (recv_ret > 0);
			std::cout << CLR_YEL << "[cgi output start]\n";
			std::cout << CLR_NON;
			std::cout << cgi.output_string;
			std::cout << CLR_YEL << "\n[cgi output end]" << CLR_NON << "\n";
			close(cgi.input);
			close(cgi.output);
			return (true);
		}
		else if (wait_res < 0) { //Note from Tutku: error here, else never works, if case only returns 1 or 0
			// brr brr error
			return (-1);
		}
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
	int			p_status = 0;
	int			got_output;

	got_output = gotCGIOutput(cgi, &p_status);
	if (got_output == -1) {
		return (-1);
	}
	if (got_output == true) {
		std::string	response_string = buildCGIResponseString(cgi.output_string);
		cgi.client.setResponse(response_string);
		cgi.client.getHttpClass().setState(READY_TO_SEND); // this sho
		std::cout << "checking response string:\n" << cgi.client.getResponse();
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
	//std::cout << CLR_YEL << "DEBUG:" << CLR_NON << "\n";
	for (size_t i = 0; i < vars.size(); i++) {
		//std::cout << "cgi_var " << i << ": " << vars.at(i) << '\n';
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
//	std::cout << "trying to exec binary: " << binary_name << '\n';
//	std::cout << "argv[0]: " << argv[0] << '\n';
//	std::cout << "argv[1]: " << argv[1] << '\n';
	for (size_t i = 0; i < paths.size(); i++) {
		slash_arg = paths.at(i) + "/" + binary_name;
		for (size_t j = 0; j < slash_arg.size(); j++) {
			path[j] = slash_arg.at(j);
		}
		//std::cout << "trying to exec in path: " << path << '\n';
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
	dup2(in_pipe[0], STDIN_FILENO); // add error checks on dup2?
	dup2(out_pipe[1], STDOUT_FILENO);
	close(in_pipe[0]);
	close(out_pipe[1]);
	std::cerr << "executing cgi in child...\n\n";
	tryExecveScript(client, binary_name, argv);
	std::cerr << "if you see this, there's an error\n"; // delete this
	exit(1);
}

static cgi_t	handle_parent(
	int in_pipe[2],
	int out_pipe[2],
	int child_pid
) {
	cgi_t	new_cgi;

	time_point<system_clock>	cgi_timer = system_clock::now();
	close(in_pipe[0]);
	close(out_pipe[1]);
	fcntl(in_pipe[1], O_NONBLOCK | O_CLOEXEC);
	fcntl(out_pipe[0], O_NONBLOCK | O_CLOEXEC);
	new_cgi.child_pid = child_pid;
	new_cgi.input = in_pipe[1];
	new_cgi.output = out_pipe[0];
	new_cgi.timer = cgi_timer;
	return (new_cgi);
}

std::optional<cgi_t>	executeCGI(
	Client& client,
	std::map<int, cgi_t>&	background_cgis
) {
	int	in_pipe[2];
	int	out_pipe[2];

	std::cout << GREEN << "Started CGI execution\n" << RESET;
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

	//cgi_t					newly_created_cgi_request;
	//newly_created_cgi_request.request_data = &http_instance;
	//background_cgis.push_back(newly_created_cgi_request);

	//td::tuple<int, size_t>	cgi_response; // @alisa what is this for?
	cgi_t	cgi;
	cgi_t	cgi_response_data;
	char*	argv[] { NULL, NULL, NULL };
	argv[0] = strdup(PYTHON_EXEC);
	std::cout << "cgi pass location: (" << client.getHttpClass().requestConfig.location->prefix << ")\n";
	std::cout << "cgi pass path: (" << client.getHttpClass().requestConfig.location->cgi_pass.path << ")\n";
	argv[1] = strdup((client.getHttpClass().getBuiltUri().c_str()));
	argv[2] = NULL;
	std::cout << "path to script: (" << argv[1] << ")\n";


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
		cgi = handle_parent(in_pipe, out_pipe, fork_ret);
		cgi.client = client;
		background_cgis.insert( {cgi.output, cgi} );
	}
	{	// this block is, essentially, what we need to do in the listen loop.
		// the while (1) is the stand-in for the listen loop.
	
		//while (!background_cgis.empty()) {
		//	cgi_response = checkBackgroundCGIs(background_cgis);
		//	const auto& [cgi_responded, cgi_index] = cgi_response;
		//	if (cgi_responded == 1) {
		//		cgi_response_data = background_cgis.at(cgi_index);
		//		background_cgis.erase(background_cgis.begin() + cgi_index);
		//		break ;
		//	}
		//	else if (cgi_responded == -1) {
		//		// brr brr error
		//		// we can do an error response and erase the background_cgi that gave error
		//		return std::nullopt;
		//	}
		//}
		// at the end of the program, run this for every previously launched cgi
		// cout message unnecessary

		//while (waitpid(cgi.child_pid, NULL, WNOHANG) == 0);
		//std::cout << "process terminated\n";
	}
	// if response has Status (case insensitive) header, than that's the status
	// otherwise, 200
	// 
	// MAKE THESE REASONABLE ->
	delete argv[0];
	delete argv[1];
	return (cgi);
}
