#include "Http.hpp"
#include "httpBuffer.hpp"
#include <iostream>
#include <cassert>
#include <cstring>

void testBufferGetRequest() {
	httpBuffer buf;
	const char *req = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
	buf.append((char*)req, strlen(req));
	assert(buf.checkStatus() == COMPLETE);
	std::cout << "Buffer GET request: OK" << std::endl;
}

void testBufferPostWithBody() {
	httpBuffer buf;
	const char *req = "POST / HTTP/1.1\r\nContent-Length: 5\r\n\r\nhello";
	buf.append((char*)req, strlen(req));
	assert(buf.checkStatus() == COMPLETE);
	std::cout << "Buffer POST with body: OK" << std::endl;
}

void testBufferIncomplete() {
	httpBuffer buf;
	const char *req = "POST / HTTP/1.1\r\nContent-Length: 10\r\n\r\nhello";
	buf.append((char*)req, strlen(req));
	assert(buf.checkStatus() == INCOMPLETE);
	std::cout << "Buffer incomplete detection: OK" << std::endl;
}

void testBufferBodyWithoutContentLen() {
	httpBuffer buf;
	const char *req = "POST / HTTP/1.1\r\n\r\nbody";
	buf.append((char*)req, strlen(req));
	assert(buf.checkStatus() == RECV_ERROR);
	std::cout << "Buffer body without Content-Length: OK" << std::endl;

}

void testParserGetRequest() {
	Http http;
	http.parseRequest("GET /api/users HTTP/1.1\r\nHost: example.com\r\n\r\n");
	assert(http.getMethod() == GET);
	assert(http.getUri() == "/api/users");
	std::cout << "Parser GET request" << std::endl;
	http.debugPrint();

}

void testParserPostWithBody() {
	Http http;
	http.parseRequest("POST / HTTP/1.1\r\nContent-Length: 5\r\n\r\nhello");
	assert(http.getMethod() == POST);
	assert(http.getContentLen() == 5);
	assert(http.getBody() == "hello");
	http.debugPrint();
	std::cout << "Parser POST with body" << std::endl;
	http.debugPrint();

}

void testParserHttp2Version() {
	Http http;
	http.parseRequest("GET / HTTP/2\r\nHost: localhost\r\n\r\n");
	assert(http.getVersion() == HTTP_2);
	std::cout << "Parser HTTP/2 detection" << std::endl;
	http.debugPrint();

}

void testParserBodySizeMismatch() {
	Http http;
	try {
		http.parseRequest("POST / HTTP/1.1\r\nContent-Length: 5\r\n\r\nhelloworld");
		assert(false);
	} catch (Http::HttpException &e) {
		std::cout << "Parser body size mismatch" << std::endl;
		http.debugPrint();

	}
}

int main() {
	std::cout << "\nBuffer tests:" << std::endl;
	testBufferGetRequest();
	testBufferPostWithBody();
	testBufferIncomplete();
	testBufferBodyWithoutContentLen();
	
	std::cout << "\nParser tests:" << std::endl;
	testParserGetRequest();
	testParserPostWithBody();
	testParserHttp2Version();
	testParserBodySizeMismatch();
	
	return 0;
}
