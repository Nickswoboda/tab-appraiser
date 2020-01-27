#include "Http.h"

#include <iostream>

Http::Http()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

Http::~Http()
{
	for (auto& handle : curl_handles_) {
		curl_easy_cleanup(handle);
	}

	curl_global_cleanup();
}

size_t Http::WriteToString(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	std::string& text = *static_cast<std::string*>(userdata);
	size_t totalsize = size * nmemb;
	text.append(static_cast<char*>(buffer), totalsize);
	return totalsize;
}

std::string Http::GetData(CURL* handle, const std::string& url)
{
	std::string result;

	curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &result);
	int error_code = curl_easy_perform(handle);
	if (error_code != CURLE_OK) {
		std::cout << "Unable to perform Http Request. Error Code: " << error_code << "\n";
	}

	return result;
}

void Http::SetCookie(CURL* handle, const std::string& cookie)
{
	curl_easy_setopt(handle, CURLOPT_COOKIE, cookie.c_str());
}

void Http::SetVerbose(CURL* handle, bool verbose)
{
	curl_easy_setopt(handle, CURLOPT_VERBOSE, verbose);
}

CURL* Http::CreateHandle()
{
	auto handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, Http::WriteToString);
	return handle;
}
