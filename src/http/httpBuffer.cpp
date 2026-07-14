#include "../../inc/httpBuffer.hpp"

//*======CONSTRUCTOR======*/
httpBuffer::httpBuffer()
	:totalBytesReceived(0)
{}

/// @brief Appends the current buffer string received to the bufferObj.recvStr. 
/// Why? Because the current buffer string (buffer) gets overwritten every call and 
///we therefore must append it to a persistent string (bufferObj.recvStr)
/// @param buffer the current buffer string from recv 
/// @param size the size of the current buffer string from recv
void httpBuffer::append(char *buffer, ssize_t size)
{
	this->recvStr.append(buffer, size);
}

/// @brief checkStatus() only checks if the bufferObj.recvStr is a complete or incomplete request. 
//  It accesses the bufferObj.recvStr with this-> because it is in the same class.
//. The white space trim happens inside the parser
/// @return COMPLETE, INCOMPLETE 
receiveStatus httpBuffer::checkStatus() 
{
	int contLenCnt = -1;
	bool hasContentLen = false;
	
	size_t headerEnd = this->recvStr.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return (INCOMPLETE);
	
	std::string headers = this->recvStr.substr(0, headerEnd);
	size_t contentLenPos = headers.find("Content-Length:");
	
	int bodyStart = headerEnd + 4;
	int bodyReceived = this->recvStr.size() - bodyStart;
	
	if (contentLenPos != std::string::npos)
	{
		hasContentLen = true;
		sscanf(headers.c_str() + contentLenPos, "Content-Length: %d", &contLenCnt);
	}

	if (!hasContentLen && bodyReceived > 0)
		return RECV_ERROR;
	

	if (hasContentLen && contLenCnt == 0 && bodyReceived > 0)
		return RECV_ERROR;
	

	if (hasContentLen && bodyReceived >= contLenCnt)
		return COMPLETE;
	
	if (!hasContentLen)
		return COMPLETE;
	
	return (INCOMPLETE);
}
