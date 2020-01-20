#include "ApiHandler.h"

void ApiHandler::SetPOESESSIDCookie()
{
	http_.SetCookie("POESESSID=" + user_.POESESSID_);
}

std::string ApiHandler::GetAccountName()
{
	SetPOESESSIDCookie();

	auto data = http_.GetData("https://www.pathofexile.com/my-account");
	std::string temp = "/account/view-profile/";
	const int temp_length = 22;

	auto temp_pos = data.find(temp);
	if (temp_pos != std::string::npos) {
		auto substring = data.substr(temp_pos + temp_length, 100);

		std::string acc_name;
		for (auto& character : substring) {
			if (character == '\"') {
				return acc_name;
			}
			acc_name += character;
		}
	}
	return "Unable to find account name";
}

std::vector<std::string> ApiHandler::GetCurrentLeagues()
{
	auto data = http_.GetData("http://api.pathofexile.com/leagues");
	auto league_info = nlohmann::json::parse(data);
	std::vector<std::string> leagues;
	for (const auto& league : league_info) {
		leagues.push_back(league["id"]);
	}

	return leagues;
}
