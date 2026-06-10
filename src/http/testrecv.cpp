#include <iostream>
#include <cstring>

enum receiveStatus{
	SOCKET_CLOSED,
	RECV_ERROR,
	COMPLETE,
	INCOMPLETE,
	MAXBYTESRECEIVED
};

class httpBuffer {
	private:
	std::string				recvStr;
	public:
	httpBuffer();
	static const int		maxRequest = 8192;
	ssize_t					totalBytesReceived;
	receiveStatus			currentBufferStatus;
	bool					isBufferComplete;
	receiveStatus			checkStatus();
	void					reset();
	void					append(char *buffer, ssize_t size);
};

httpBuffer::httpBuffer()
	:totalBytesReceived(0), isBufferComplete(false)
{}

void httpBuffer::append(char *buffer, ssize_t size)
{
	this->recvStr.append(buffer, size);
}

receiveStatus httpBuffer::checkStatus()
{
	const char	*contLenStr = "Content-Length:";
	const char	*contLenPtr;
	int			contLenCnt = 0;
	int			headerEnd = 0;
	int			bodyReceived = 0;

	size_t pos = this->recvStr.find("\r\n\r\n");
	if (pos == std::string::npos)
		return (this->currentBufferStatus = INCOMPLETE);
	contLenPtr = strstr(this->recvStr.c_str(), contLenStr);
	if (!contLenPtr)
	{
		this->currentBufferStatus = COMPLETE;
		return COMPLETE;
	}
	sscanf(contLenPtr, "Content-Length: %d", &contLenCnt);
	headerEnd = pos + strlen("\r\n\r\n");
	bodyReceived = this->recvStr.size() - headerEnd;
	if (bodyReceived >= contLenCnt)
	{
		this->currentBufferStatus = COMPLETE;
		return COMPLETE;
	}
	return (this->currentBufferStatus = INCOMPLETE);
}

int main()
{
	// Test 1: GET request (no body)
	std::cout << "=== Test 1: GET Request ===" << std::endl;
	httpBuffer buf1;
	const char *getRequest = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
	buf1.append((char*)getRequest, strlen(getRequest));
	buf1.checkStatus();
	std::cout << "Status: " << buf1.currentBufferStatus << " (0=COMPLETE expected)" << std::endl;
	std::cout << std::endl;

	// Test 2: POST request with Content-Length
	std::cout << "=== Test 2: POST Request (Full) ===" << std::endl;
	httpBuffer buf2;
	const char *postRequest = "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 11\r\n\r\nHello World";
	buf2.append((char*)postRequest, strlen(postRequest));
	buf2.checkStatus();
	std::cout << "Status: " << buf2.currentBufferStatus << " (0=COMPLETE expected)" << std::endl;
	std::cout << std::endl;

	// Test 3: POST request with incomplete body (simulate chunked recv)
	std::cout << "=== Test 3: POST Request (Incomplete) ===" << std::endl;
	httpBuffer buf3;
	const char *postHeaders = "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 11\r\n\r\nHello";
	buf3.append((char*)postHeaders, strlen(postHeaders));
	buf3.checkStatus();
	std::cout << "Status: " << buf3.currentBufferStatus << " (1=INCOMPLETE expected)" << std::endl;
	std::cout << "Now adding rest of body..." << std::endl;
	const char *restOfBody = " World";
	buf3.append((char*)restOfBody, strlen(restOfBody));
	buf3.checkStatus();
	std::cout << "Status: " << buf3.currentBufferStatus << " (0=COMPLETE expected)" << std::endl;

	return 0;
}