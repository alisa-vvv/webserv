#include <string>
#include "Http.hpp"
#include <time.h>

enum receiveStatus{
	SOCKET_CLOSED,
	RECV_ERROR,
	COMPLETE,
	INCOMPLETE,
	MAXBYTESRECEIVED,
	TIMEOUT
};

class httpBuffer {
	private:
		std::string				recvStr;

	public:
		httpBuffer();
		static const int		maxRequest = 8192;
		ssize_t					totalBytesReceived;
		receiveStatus			checkStatus();
		void					append(char *buffer, ssize_t size);
		const char*				getRecvStr() const { return this->recvStr.c_str(); }
};
