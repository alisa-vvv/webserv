/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RcvBuffer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 12:57:21 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/08 15:28:38 by tcakir-y         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RcvBuffer.hpp"

//*======CONSTRUCTOR======*/
RcvBuffer::RcvBuffer()
	:totalBytesReceived(0)
{}

/// @brief Appends the current buffer string received to the bufferObj.recvStr. 
/// Why? Because the current buffer string (buffer) gets overwritten every call and 
///we therefore must append it to a persistent string (bufferObj.recvStr)
/// @param buffer the current buffer string from recv 
/// @param size the size of the current buffer string from recv
void RcvBuffer::append(char *buffer, ssize_t size)
{
	this->recvStr.append(buffer, size);
}

/// @brief checkStatus() only checks if the bufferObj.recvStr is a complete or incomplete request. 
//  It accesses the bufferObj.recvStr with this-> because it is in the same class.
//. The white space trim happens inside the parser
/// @return COMPLETE, INCOMPLETE 
receiveStatus RcvBuffer::checkStatus() 
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
	

	if (hasContentLen && bodyReceived == contLenCnt)
		return COMPLETE;
	else
	{
		if (!hasContentLen)
			return COMPLETE;
		return INCOMPLETE;
	}
	return (INCOMPLETE);
}

//We gotta add the recv to the socket loop, we can just take the while loop?
//the httpBuffer class is in httpBuffer.hpp, the main adding to the 

/// @brief This is where the recv function lives. We need to refactor because the recv needs to be in the main poll loop.
/// @param bufferObj 
/// @param sockFd 
///// @return COMPLETE, INCOMPLETE, TIMEOUT, SOCKET_CLOSED, RECV_ERRIR
//receiveStatus recvHttpRequest(RcvBuffer &bufferObj, int sockFd) //must take socket as param
//{
//	ssize_t			bytesRead;
//	char			buffer[4096];	
//	static const int		TIMEOUTCONST = 30;
//	time_t					lastActivity = time(NULL);

//	while (1)
//	{
//		//set http.setState(RECEIVING);
//		bytesRead = recv(sockFd, buffer, sizeof(buffer), 0); //recv the string, add to buffer.
//		//the buffer gets overwritten every call
//		if (bytesRead == 0) // if 0, closed
//			return SOCKET_CLOSED;
//		if (bytesRead < 0) // if -1 recv errir
//			return RECV_ERROR;
//		lastActivity = time(NULL); //change last activity to now

//		bufferObj.totalBytesReceived += bytesRead; //add recv bytes read to total bytes read
//		bufferObj.append(buffer, bytesRead); //append the buffer to the string inside the bufferObj
//		if (bufferObj.checkStatus()== COMPLETE) // //check if the request is complete with headers. 
//			return COMPLETE;
//		if (time(NULL) - lastActivity > TIMEOUTCONST) //timeout check
//			return TIMEOUT;
//		//if incomplete, keep recving. but this is probably what we need to chnge -> recv must be outside the loop
//	}
//	return MAXBYTESRECEIVED;
//}