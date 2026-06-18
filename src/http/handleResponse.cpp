#include "Http.hpp"

void Http::handleGet()
{
	
}

void Http::handlePost()
{
	
}

void Http::handleDelete()
{
	
}

void Http::handleResponse()
{
	if (this->_method == GET)
		handleGet();
	else if (this->_method == POST)
		handlePost();
	else if (this->_method == DELETE)
		handleDelete();
	
}