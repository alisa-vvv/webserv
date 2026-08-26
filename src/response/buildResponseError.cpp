#include "../../inc/Http.hpp"
#include <fstream>

void Http::handleErrorResponse()
{
	int stat = getStatusCode();
	if (requestConfig.server != NULL)
	{
		std::map<int, std::string>::const_iterator it =
			requestConfig.server->error_pages.find(stat);

		if (it != this->requestConfig.server->error_pages.end())
		{
			setBody(it->second);
			if (getState() == CLIENT_ERROR)
				return;
				
			setResponseHeader("Content-Type", "text/html");
			setState(READY_TO_SEND);
			return;
		}
	}
	setBody(std::get<1>(HTTP_STATUS_MESSAGE.at(stat)));
	setResponseHeader("Content-Type", "text/html");
	setState(READY_TO_SEND);
}
