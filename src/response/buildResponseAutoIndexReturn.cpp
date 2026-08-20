#include "../../inc/Http.hpp"


void Http::handleAutoIndexResponse() 
{
	try
	{
		std::filesystem::path dir(this->_builtUri);
		std::string body = "<html><body><ul>";
		for (const auto &entry : std::filesystem::directory_iterator(dir))
		{
			std::string name = entry.path().filename().string();
			body += "<li><a href=\"" + name + "\">" + name + "</a></li>";
		}
		body += "</ul></body></html>";
		this->_body = body;
		setResponseCode(HTTP_OK);
		setResponseHeader("Content-Type", "text/html");
		setState(READY_TO_SEND);
	}
	catch(const std::exception& e)
	{
		setResponseCode(HTTP_FORBIDDEN);
	}
}

void Http::handleReturnResponse()
{
	int returnCode = requestConfig.location->returns.code;
	if (returnCode < 300 || returnCode > 399)
		returnCode = HTTP_MOVED_PERMANENTLY;
	std::cout << CLR_YEL << "DOING SHIT" << CLR_NON << "\n";
	setResponseCode(returnCode);
	setResponseHeader("Location", requestConfig.location->returns.target);
	setBody("");
	setState(READY_TO_SEND);
}
