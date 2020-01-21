#pragma once

#include "UserData.h"
#include "Http.h"
#include <string>
#include <vector>

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

	Http http_;
	UserData& user_;

};