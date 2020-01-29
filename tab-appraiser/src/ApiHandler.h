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
	std::unordered_map<std::string, float> GetCurrencyPriceData(const char* item_type);
	std::unordered_map<std::string, float> GetItemPriceData(const char* item_type);
	std::unordered_map<std::string, float> GetPriceData(const std::string& league);

	Http http_;
	CURL* poe_curl_handle_;
	CURL* ninja_curl_handle_;
	UserData& user_;
	std::string error_msg_;

};