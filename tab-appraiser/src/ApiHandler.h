#pragma once

#include "UserData.h"
#include "Http.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

class ApiHandler
{
public:

	ApiHandler(UserData& user);
	void SetPOESESSIDCookie();
	std::string GetAccountName();
	std::vector<std::string> GetCurrentLeagues();
	std::vector<std::string> GetStashTabList();
	std::vector<std::string> GetStashItems(int index);
	std::unordered_map<std::string, float> GetPriceData(const char* item_type);
	
	Http http_;
	CURL* poe_curl_handle_;
	CURL* ninja_curl_handle_;
	UserData& user_;
	std::string league_encoded_;
	const std::string ninja_base_url = "https://poe.ninja/api/data/";
};