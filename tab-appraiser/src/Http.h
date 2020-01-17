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

	nlohmann::json Get(const std::string& url);

private:
	CURL* curl;

	static size_t WriteToJson(void* buffer, size_t size, size_t nmemb, void* userdata);
};