#include "Http.hpp"
#include <filesystem>

std::map<std::string, std::string> MIME_TYPE {
	{".html", "text/html"},
	{".htm", "text/html"},
	{".css", "text/css"},
	{".gif", "image/gif"},
	{".jpeg", "image/jpeg"},
	{".jpg", "image/jpeg"},
	{".js", "application/javascript"},
	{".json", "application/json"},
	{".md", "text/markdown"},
	{".png", "image/png"},
	{".pdf","application/pdf"},
	{".php", "application/x-httpd-php"},
	{".txt", "text/plain"},
	{".xml", "application/xml"},
	{".bin", "application/octet-stream"},
	{".exe", "application/octet-stream"},
	{".dll", "application/octet-stream"},
	{".mp4", "video/mp4"},
	{".svg", "image/svg+xml"}
};

/// @brief Set the content type from path or mode
void Http::setContentType()
{
	std::filesystem::path path(this->_uri);
	std::string extension = path.extension().string();
	std::map<std::string, std::string>::iterator it = MIME_TYPE.find(extension);
	if (it != MIME_TYPE.end())
		setResponseHeader("Content-Type", it->second);
	else
		setResponseHeader("Content-Type", "application/octet-stream"); //fallback
	
}