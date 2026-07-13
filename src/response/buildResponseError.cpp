#include "../../inc/Http.hpp"
#include <fstream>

void Http::handleErrorResponse()
{
	int stat = getStatusCode();
	bool found = false;
	for (const t_error_page &page: requestConfig->server->error_pages)
	{
		if (stat == page.error_num)
		{
			setResponseHeader("Location:", page.redirect);
			found = true;
			break;
		}
	}
	if (!found)
	{
		setResponseHeader("Location:", std::get<1>(HTTP_STATUS_MESSAGE.at(stat)));
	}
	setContentType();
	//if error pages are configured 
}