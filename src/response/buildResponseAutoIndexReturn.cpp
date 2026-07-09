#include "Http.hpp"


void Http::handleAutoIndexResponse()
{
	try
	{
		std::filesystem::path dir(this->_uri);
		std::string body = "<html><body><ul>";
		for (const auto &entry : std::filesystem::directory_iterator(dir))
		{
			std::string name = entry.path().filename().string();
			body += "<li><a href=\"" + name + "\">" + name + "</a></li>";
		}
		body += "</ul></body></html>";
		setBody(body);
		setResponseCode(HTTP_OK);
		setResponseHeader("Content-Type", "test/html");
		setState(READY_TO_SEND);
	}
	catch(const std::exception& e)
	{
		setResponseCode(HTTP_FORBIDDEN);
	}
}

void Http::handleReturnResponse()
{ //ticket15
	if (requestConfig->location->returns->status > 300 && requestConfig->location->returns->status < 400)
		return setResponseCode(HTTP_INTERNAL_SERVER_ERROR);
	setResponseCode(HTTP_MOVED_PERMANENTLY);
	setResponseHeader("Location:", requestConfig->location->returns->target);
	setBody("");
	setState(READY_TO_SEND);
}