#pragma once

#define CURL_STATICLIB
#include <curl/curl.h>
#include <json.hpp>

#include <string>
class Http
{
public:
	Http();
	~Http();

	std::string GetData(const std::string& url);
	void SetCookie(const std::string& cookie);

private:
	CURL* curl;

	static size_t WriteToString(void* buffer, size_t size, size_t nmemb, void* userdata);
};