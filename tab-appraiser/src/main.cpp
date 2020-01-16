#define CURL_STATICLIB
#include <iostream>
#include <string>

#include "curl/curl.h"

static size_t my_write(void* buffer, size_t size, size_t nmemb, void* param)
{
	std::string& text = *static_cast<std::string*>(param);
	size_t totalsize = size * nmemb;
	text.append(static_cast<char*>(buffer), totalsize);
	return totalsize;
}

int main()
{
	std::string result;
	CURL* curl = curl_easy_init();
	curl_global_init(CURL_GLOBAL_DEFAULT);
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://poe.ninja");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		CURLcode success = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (CURLE_OK != success) {
			std::cerr << "CURL error: " << success << '\n';
		}
	}
	curl_global_cleanup();
	std::cout << result << "\n\n";
}