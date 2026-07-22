#include "../inc/Http.hpp"
#include <iostream>

const char *methodToString(httpMethod method)
{
	switch(method)
	{
		case GET: return "GET";
		case POST: return "POST";
		case DELETE: return "DELETE";
		case UNKNOWN: return "UNKNOWN";
		default: return "?";
	}
}

const char *versionToString(httpVersion version)
{
	switch(version)
	{
		case HTTP_1_0: return "HTTP/1.0";
		case INVALID: return "INVALID";
		default: return "?";
	}
}

const char *stateToString(clientState state)
{
	switch(state)
	{
		case HANDLING_CGI_EXTENSION: return "HANDLING_CGI_EXTENSION";
		case HANDLING_CGI_STATIC: return "HANDLING_CGI_STATIC";
		case READY_TO_SEND: return "READY_TO_SEND";
		case CLIENT_ERROR: return "CLIENT_ERROR";
		default: return "?";
	}
}

int main(void)
{
	// Create a mock HTTP request string (GET request)
	std::string mockRequest = 
		"GET /index.html HTTP/1.0\r\n"
		"Host: localhost:8080\r\n"
		"User-Agent: TestClient/1.0\r\n"
		"Connection: close\r\n"
		"\r\n";

	std::cout << "\n========== TEST 1: GET Request ==========\n" << std::endl;

	// Create Http object
	Http httpClient;

	// Parse the mock request
	std::cout << "1. Parsing mock HTTP request..." << std::endl;
	std::cout << "   Raw Request:\n" << mockRequest << std::endl;
	httpClient.parseRequest(mockRequest);
	std::cout << "   ✓ Parsed successfully" << std::endl;
	std::cout << "   Method: " << methodToString(httpClient.getMethod()) << std::endl;
	std::cout << "   Built URI: " << httpClient.getBuiltUri() << std::endl;
	std::cout << "   Version: " << versionToString(httpClient.getVersion()) << std::endl;
	std::cout << "   Status Code: " << httpClient.getStatusCode() << std::endl;

	// Create a minimal mock config
	std::cout << "\n2. Setting up mock config..." << std::endl;
	cfg_server_t mockServer;
	mockServer.ip_addr = INADDR_ANY;
	mockServer.ports.push_back(8080);
	mockServer.root = "./server";
	mockServer.client_max_body_size = 10000000;
	mockServer.autoindex = true;

	t_location mockLocation;
	mockLocation.prefix = "/";
	mockLocation.root = "./server";
	mockLocation.index = "index.html";
	mockLocation.autoindex = true;
	mockLocation.allowed_methods[GET] = true;
	mockLocation.allowed_methods[POST] = true;
	mockLocation.allowed_methods[DELETE] = true;

	mockServer.locations.push_back(mockLocation);

	// Manually set request config on the Http object
	requestConfig mockReqConfig;
	mockReqConfig.server = &mockServer;
	mockReqConfig.location = &mockServer.locations[0];
	httpClient.requestConfig = new requestConfig(mockReqConfig);

	std::cout << "   Server root: " << mockServer.root << std::endl;
	std::cout << "   Location prefix: " << mockLocation.prefix << std::endl;

	// Validate the request
	std::cout << "\n3. Validating request..." << std::endl;
	httpClient.validateLayer();
	std::cout << "   Status Code after validation: " << httpClient.getStatusCode() << std::endl;
	std::cout << "   Client State: " << stateToString(httpClient.getState()) << std::endl;

	// Build response
	std::cout << "\n4. Building response..." << std::endl;
	httpClient.buildResponse();
	std::cout << "   Status Code: " << httpClient.getStatusCode() << std::endl;

	// Build response string
	std::cout << "\n5. Building response string..." << std::endl;
	httpClient.buildResponseString();
	std::cout << "   ✓ Response string built" << std::endl;

	// Print the full response
	std::cout << "\n========== RESPONSE ==========\n" << std::endl;
	std::cout << httpClient.getResponseString();
	std::cout << "\n========== END RESPONSE ==========\n" << std::endl;

	// Test with a POST request
	std::cout << "\n========== TEST 2: POST Request ==========\n" << std::endl;

	std::string postRequest = 
		"POST /upload HTTP/1.0\r\n"
		"Host: localhost:8080\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 16\r\n"
		"Connection: close\r\n"
		"\r\n"
		"{\"test\":\"data\"}";

	Http httpClient2;
	std::cout << "1. Parsing POST request..." << std::endl;
	std::cout << "   Raw Request:\n" << postRequest << std::endl;
	httpClient2.parseRequest(postRequest);
	std::cout << "   ✓ Parsed successfully" << std::endl;
	std::cout << "   Method: " << methodToString(httpClient2.getMethod()) << std::endl;
	std::cout << "   Built URI: " << httpClient2.getBuiltUri() << std::endl;
	std::cout << "   Content-Length: " << httpClient2.getContentLen() << std::endl;
	std::cout << "   Has Body: " << (httpClient2.getBody().empty() ? "NO" : "YES") << std::endl;

	httpClient2.requestConfig = new requestConfig(mockReqConfig);

	std::cout << "\n2. Validating request..." << std::endl;
	httpClient2.validateLayer();
	std::cout << "   Status Code: " << httpClient2.getStatusCode() << std::endl;
	std::cout << "   Client State: " << stateToString(httpClient2.getState()) << std::endl;

	std::cout << "\n3. Building response..." << std::endl;
	httpClient2.buildResponse();
	std::cout << "   Status Code: " << httpClient2.getStatusCode() << std::endl;

	std::cout << "\n4. Building response string..." << std::endl;
	httpClient2.buildResponseString();
	std::cout << "   ✓ Response string built" << std::endl;

	std::cout << "\n========== POST RESPONSE ==========\n" << std::endl;
	std::cout << httpClient2.getResponseString();
	std::cout << "\n========== END RESPONSE ==========\n" << std::endl;

	delete httpClient.requestConfig;
	delete httpClient2.requestConfig;

	// TEST 3: Missing Host header (should get 400 Bad Request)
	std::cout << "\n========== TEST 3: Missing Host Header ==========\n" << std::endl;

	std::string noHostRequest = 
		"GET /index.html HTTP/1.0\r\n"
		"User-Agent: TestClient/1.0\r\n"
		"\r\n";

	Http httpClient3;
	std::cout << "1. Parsing request without Host header..." << std::endl;
	httpClient3.parseRequest(noHostRequest);
	std::cout << "   Method: " << methodToString(httpClient3.getMethod()) << std::endl;

	cfg_server_t mockServer3 = mockServer;
	requestConfig mockReqConfig3;
	mockReqConfig3.server = &mockServer3;
	mockReqConfig3.location = &mockServer3.locations[0];
	httpClient3.requestConfig = new requestConfig(mockReqConfig3);

	std::cout << "\n2. Validating request..." << std::endl;
	httpClient3.validateLayer();
	std::cout << "   Status Code: " << httpClient3.getStatusCode() << std::endl;
	std::cout << "   Expected: 400 (Bad Request - missing Host)" << std::endl;

	std::cout << "\n3. Building response..." << std::endl;
	httpClient3.buildResponse();
	httpClient3.buildResponseString();

	std::cout << "\n========== RESPONSE ==========\n" << std::endl;
	std::cout << httpClient3.getResponseString();
	std::cout << "\n========== END RESPONSE ==========\n" << std::endl;

	delete httpClient3.requestConfig;

	// TEST 4: Invalid HTTP version (should get 505)
	std::cout << "\n========== TEST 4: Invalid HTTP Version ==========\n" << std::endl;

	std::string badVersionRequest = 
		"GET /index.html HTTP/2.0\r\n"
		"Host: localhost:8080\r\n"
		"\r\n";

	Http httpClient4;
	std::cout << "1. Parsing request with HTTP/2.0..." << std::endl;
	httpClient4.parseRequest(badVersionRequest);
	std::cout << "   Version: " << versionToString(httpClient4.getVersion()) << std::endl;

	cfg_server_t mockServer4 = mockServer;
	requestConfig mockReqConfig4;
	mockReqConfig4.server = &mockServer4;
	mockReqConfig4.location = &mockServer4.locations[0];
	httpClient4.requestConfig = new requestConfig(mockReqConfig4);

	std::cout << "\n2. Validating request..." << std::endl;
	httpClient4.validateLayer();
	std::cout << "   Status Code: " << httpClient4.getStatusCode() << std::endl;
	std::cout << "   Expected: 505 (HTTP Version Not Supported)" << std::endl;

	std::cout << "\n3. Building response..." << std::endl;
	httpClient4.buildResponse();
	httpClient4.buildResponseString();

	std::cout << "\n========== RESPONSE ==========\n" << std::endl;
	std::cout << httpClient4.getResponseString();
	std::cout << "\n========== END RESPONSE ==========\n" << std::endl;

	delete httpClient4.requestConfig;

	// TEST 5: Method Not Allowed (POST when only GET allowed)
	std::cout << "\n========== TEST 5: Method Not Allowed ==========\n" << std::endl;

	std::string postToGetOnlyRequest = 
		"POST /api/data HTTP/1.0\r\n"
		"Host: localhost:8080\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 0\r\n"
		"\r\n";

	Http httpClient5;
	std::cout << "1. Parsing POST request..." << std::endl;
	httpClient5.parseRequest(postToGetOnlyRequest);

	// Create a location that only allows GET
	cfg_server_t mockServer5;
	mockServer5.root = "./server";
	t_location restrictedLocation;
	restrictedLocation.prefix = "/api";
	restrictedLocation.root = "./server";
	restrictedLocation.allowed_methods[GET] = true;
	restrictedLocation.allowed_methods[POST] = false;
	restrictedLocation.allowed_methods[DELETE] = false;
	mockServer5.locations.push_back(restrictedLocation);

	requestConfig mockReqConfig5;
	mockReqConfig5.server = &mockServer5;
	mockReqConfig5.location = &mockServer5.locations[0];
	httpClient5.requestConfig = new requestConfig(mockReqConfig5);

	std::cout << "\n2. Validating request (POST to GET-only endpoint)..." << std::endl;
	httpClient5.validateLayer();
	std::cout << "   Status Code: " << httpClient5.getStatusCode() << std::endl;
	std::cout << "   Expected: 405 (Method Not Allowed)" << std::endl;

	std::cout << "\n3. Building response..." << std::endl;
	httpClient5.buildResponse();
	httpClient5.buildResponseString();

	std::cout << "\n========== RESPONSE ==========\n" << std::endl;
	std::cout << httpClient5.getResponseString();
	std::cout << "\n========== END RESPONSE ==========\n" << std::endl;

	delete httpClient5.requestConfig;

	// TEST 6: Path Traversal Detection
	std::cout << "\n========== TEST 6: Path Traversal Attack ==========\n" << std::endl;

	std::string pathTraversalRequest = 
		"GET /../../../etc/passwd HTTP/1.0\r\n"
		"Host: localhost:8080\r\n"
		"\r\n";

	Http httpClient6;
	std::cout << "1. Parsing request with path traversal attempt..." << std::endl;
	httpClient6.parseRequest(pathTraversalRequest);
	std::cout << "   URI: " << httpClient6.getBuiltUri() << std::endl;

	cfg_server_t mockServer6 = mockServer;
	requestConfig mockReqConfig6;
	mockReqConfig6.server = &mockServer6;
	mockReqConfig6.location = &mockServer6.locations[0];
	httpClient6.requestConfig = new requestConfig(mockReqConfig6);

	std::cout << "\n2. Validating request..." << std::endl;
	httpClient6.validateLayer();
	std::cout << "   Status Code: " << httpClient6.getStatusCode() << std::endl;
	std::cout << "   Expected: 400 (Bad Request - contains ..)" << std::endl;

	std::cout << "\n3. Building response..." << std::endl;
	httpClient6.buildResponse();
	httpClient6.buildResponseString();

	std::cout << "\n========== RESPONSE ==========\n" << std::endl;
	std::cout << httpClient6.getResponseString();
	std::cout << "\n========== END RESPONSE ==========\n" << std::endl;

	delete httpClient6.requestConfig;

	// TEST 7: CGI Extension Detection (.py)
	std::cout << "\n========== TEST 7: CGI Extension Detection ==========\n" << std::endl;

	std::string cgiRequest = 
		"GET /cgi-bin/script.py HTTP/1.0\r\n"
		"Host: localhost:8080\r\n"
		"\r\n";

	Http httpClient7;
	std::cout << "1. Parsing request with .py extension..." << std::endl;
	httpClient7.parseRequest(cgiRequest);
	std::cout << "   URI: " << httpClient7.getBuiltUri() << std::endl;

	cfg_server_t mockServer7 = mockServer;
	requestConfig mockReqConfig7;
	mockReqConfig7.server = &mockServer7;
	mockReqConfig7.location = &mockServer7.locations[0];
	httpClient7.requestConfig = new requestConfig(mockReqConfig7);

	std::cout << "\n2. Validating request (which detects CGI extensions)..." << std::endl;
	httpClient7.validateLayer();
	std::cout << "   Has Extension: " << (httpClient7.getExtension() ? "YES" : "NO") << std::endl;

	delete httpClient7.requestConfig;

	return (0);
}



# HTTP class test
TEST_NAME = test_http
TEST_OFILES = $(addprefix $(BUILDDIR),test_main.o Http.o httpBuffer.o httpParsers.o httpValidate.o setRequestConfig.o clientHandler.o buildResponse.o buildResponseError.o buildResponseGet.o buildResponsePost.o buildResponseDelete.o buildResponseAutoIndexReturn.o buildResponseUtils.o Listener.o timer.o)

$(TEST_NAME): $(TEST_OFILES)
	$(CC) $(CFLAGS) -o $@ $(TEST_OFILES) $(LDFLAGS) $(INCFLAGS)

test_build: $(TEST_NAME)
test_run: test_build
	./$(TEST_NAME)

.PHONY:	clangd all clean fclean re test run leak debug gdb test_build test_run test_gdb
