#include "../../inc/Http.hpp"
#include <fstream>

void Http::handleErrorResponse()
{
	int stat = getStatusCode();
	auto it = requestConfig->server->error_pages.find(stat);
	if (it != this->requestConfig->server->error_pages.end())
	{
		setBody(it->second);
		if (getState() == CLIENT_ERROR)
			return;
	}
	else {
		setBody(std::get<1>(HTTP_STATUS_MESSAGE.at(stat)));
	}
	setResponseHeader("Content-Type", "text/html");
	setState(READY_TO_SEND);
}
