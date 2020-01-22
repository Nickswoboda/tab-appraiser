#include "Http.h"

Http::Http()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Http::WriteToString);
}

Http::~Http()
{
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

size_t Http::WriteToString(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	std::string& text = *static_cast<std::string*>(userdata);
	size_t totalsize = size * nmemb;
	text.append(static_cast<char*>(buffer), totalsize);
	return totalsize;
}

std::string Http::GetData(const std::string& url)
{
	std::string result;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
	int success = curl_easy_perform(curl);

	return result;
}

void Http::SetCookie(const std::string& cookie)
{
	curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str());
}

void Http::SetVerbose(bool verbose)
{
	curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose);
}
