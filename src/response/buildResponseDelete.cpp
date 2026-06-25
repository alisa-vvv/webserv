#include "Http.hpp"
#include <fstream>

void Http::handleDeleteResponse()
{
	//if file exists and is deletable, delete file, set status code to 200
	//else if file exists but is not deletable, set status code to 403
	//else set status code to 404 not foiund
	setState(READY_TO_SEND);
	
}
