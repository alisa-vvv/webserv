/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RcvBuffer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tutku <tutku@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 12:57:21 by tcakir-y          #+#    #+#             */
/*   Updated: 2026/07/20 23:48:29 by tutku            ###   ########.fr       */
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
receiveStatus RcvBuffer::checkStatus()
{
	size_t contLenCnt = -1;
	bool hasContentLen = false;

	size_t headerEnd = this->recvStr.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
	{
		if (recvStr.size() > MAX_REQUEST) //reject large headers
			return RECV_ERROR;
		return (INCOMPLETE);
	}
	if (headerEnd > MAX_REQUEST)
		return RECV_ERROR;
		
	std::string headers = this->recvStr.substr(0, headerEnd);
	size_t contentLenPos = headers.find("Content-Length:");

	size_t bodyStart = headerEnd + 4;
	size_t bodyReceived = this->recvStr.size() - bodyStart;

	if (contentLenPos != std::string::npos)
	{
		hasContentLen = true;
		long long parsedContentLength = -1;

		int result = sscanf(headers.c_str() + contentLenPos, "Content-Length: %lld", &parsedContentLength);

		if (result != 1 || parsedContentLength < 0)
			return RECV_ERROR;
		contLenCnt = static_cast<size_t>(parsedContentLength);
	}
	if (!hasContentLen)
	{
		if (bodyReceived > 0)
			return RECV_ERROR;
		return COMPLETE;
	}
	if (bodyReceived == contLenCnt)
		return COMPLETE;

	if (bodyReceived < contLenCnt)
		return (INCOMPLETE);
	
	return RECV_ERROR;
}
