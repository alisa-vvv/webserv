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
{
	//if code < 300 or code > 300
	//error internal server
	//setresponsecode to redirect code
	//set responseheader ("Location", redirect target)
	//setbody to empty
	//setstate ready to send
}