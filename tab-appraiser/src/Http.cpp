#include "Http.h"

Http::Http()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Http::WriteToJson);
}

Http::~Http()
{
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

size_t Http::WriteToJson(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	std::string& text = *static_cast<std::string*>(userdata);
	size_t totalsize = size * nmemb;
	text.append(static_cast<char*>(buffer), totalsize);
	return totalsize;
}

nlohmann::json Http::Get(const std::string& url)
{
	std::string result;
	
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	int success = curl_easy_perform(curl);
	auto json = nlohmann::json::parse(result);
	return json;
}
