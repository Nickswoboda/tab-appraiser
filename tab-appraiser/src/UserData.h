#pragma once

#include <string>

class UserData
{
public:
	void Save();
	void Load();

	std::string account_name_;
	std::string selected_league_;
	std::string POESESSID_;
};