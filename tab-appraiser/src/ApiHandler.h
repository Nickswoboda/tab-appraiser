#pragma once

#include "UserData.h"
#include "Http.h"

#include <string>
#include <vector>
#include <unordered_map>
class ApiHandler
{
public:
	ApiHandler(UserData& user)
		: user_(user) {}
	void SetPOESESSIDCookie();
	std::string GetAccountName();
	std::vector<std::string> GetCurrentLeagues();
	std::vector<std::string> GetStashTabList();
	std::vector<std::string> GetStashItems(int index);
	std::unordered_map<std::string, float> GetPriceData(const std::string& league);

	Http http_;
	UserData& user_;

};