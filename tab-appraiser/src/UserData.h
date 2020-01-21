#pragma once

#include <string>
#include <vector>

class UserData
{
public:
	void Save();
	void Load();

	std::string account_name_;
	std::string selected_league_;
	std::string POESESSID_;
	std::vector<std::string> stash_tab_list_;
};