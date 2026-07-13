#include "../../inc/Http.hpp"
#include <fstream>

void Http::handleErrorResponse()
{
	int stat = getStatusCode();
	auto it = requestConfig->server->error_pages.find(stat);
	if (it != this->requestConfig->server->error_pages.end())
	{
		setResponseHeader("Location:", it->second);
		setBody(it->second);
	}
	else
		setResponseHeader("Location:", std::get<1>(HTTP_STATUS_MESSAGE.at(stat)));
		setBody(std::get<1>(HTTP_STATUS_MESSAGE.at(stat)));
	setContentType();
}