#pragma once

#define CURL_STATICLIB
#include <curl/curl.h>

#include <string>
#include <vector>

class Http
{
public:
	Http();
	~Http();

	std::string GetData(CURL*, const std::string& url);
	void SetCookie(CURL* handle, const std::string& cookie);
	void SetVerbose(CURL*, bool verbose);
	CURL* CreateHandle();
private:
	std::vector<CURL*> curl_handles_;
	static size_t WriteToString(void* buffer, size_t size, size_t nmemb, void* userdata);
};