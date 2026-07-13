/* ************************************************************************** */
/*                                                                            */
/*                                                            ::::::::        */
/*   cgi_exec.hpp                                            :+:    :+:       */
/*                                                          +:+               */
/*   By: avaliull <avaliull@student.codam.nl>              +#+                */
/*                                                        +#+                 */
/*   Created: 2026/06/25 13:00:51 by avaliull            #+#    #+#           */
/*   Updated: 2026/06/25 13:02:57 by avaliull            ########   odam.nl   */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "configParser.hpp"

int	executeCGI(const cfg_server_t& server_config);

//DOCUMENT_ROOT		The root directory of your server
//HTTP_COOKIE		The visitor's cookie, if one is set
//HTTP_HOST			The hostname of the page being attempted
//HTTP_REFERER		The URL of the page that called your program
//HTTP_USER_AGENT	The browser type of the visitor
//HTTPS				"on" if the program is being called through a secure server
//PATH				The system path your server is running under
//QUERY_STRING		The query string (see GET, below)
//REMOTE_ADDR		The IP address of the visitor
//REMOTE_HOST		The hostname of the visitor (if your server has reverse-name-lookups on; otherwise this is the IP address again)
//REMOTE_PORT		The port the visitor is connected to on the web server
//REMOTE_USER		The visitor's username (for .htaccess-protected pages)
//REQUEST_METHOD	GET or POST
//REQUEST_URI		The interpreted pathname of the requested document or CGI (relative to the document root)
//SCRIPT_FILENAME	The full pathname of the current CGI
//SCRIPT_NAME		The interpreted pathname of the current CGI (relative to the document root)
//SERVER_ADMIN		The email address for your server's webmaster
//SERVER_NAME		Your server's fully qualified domain name (e.g. www.cgi101.com)
//SERVER_PORT		The port number your server is listening on
//SERVER_SOFTWARE	The server software you're using (e.g. Apache 1.3)
typedef struct t_envVarCGI {
	const std::string	cgi_prefix_document_root = "DOCUMENT_ROOT=";
	const std::string	cgi_prefix_http_cookie = "HTTP_COOKIE=";
	const std::string	cgi_prefix_http_host = "HTTP_HOST=";
	const std::string	cgi_prefix_http_referer = "HTTP_REFERER=";
	const std::string	cgi_prefix_http_user_agent = "HTTP_USER_AGENT=";
	const std::string	cgi_prefix_https = "HTTPS=";
	const std::string	cgi_prefix_path = "PATH=";
	const std::string	cgi_prefix_query_string = "QUERY_STRING=";
	const std::string	cgi_prefix_remote_addr = "REMOTE_ADDR=";
	const std::string	cgi_prefix_remote_host = "REMOTE_HOST=";
	const std::string	cgi_prefix_remote_port = "REMOTE_PORT=";
	const std::string	cgi_prefix_remote_user = "REMOTE_USER=";
	const std::string	cgi_prefix_request_method = "REQUEST_METHOD=";
	const std::string	cgi_prefix_request_uri = "REQUEST_URI=";
	const std::string	cgi_prefix_script_filename = "SCRIPT_FILENAME=";
	const std::string	cgi_prefix_script_name = "SCRIPT_NAME=";
	const std::string	cgi_prefix_server_admin = "SERVER_ADMIN=";
	const std::string	cgi_prefix_server_name = "SERVER_NAME=";
	const std::string	cgi_prefix_server_port = "SERVER_PORT=";
	const std::string	cgi_prefix_server_software = "SERVER_SOFTWARE=";
} t_envVarCGI;

int	executeCGI(
	const cfg_server_t& server_config
);
