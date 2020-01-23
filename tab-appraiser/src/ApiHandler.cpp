#include "ApiHandler.h"

#include <fstream>
#include <iomanip>
#include <filesystem>
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
		std::string league_name = league["id"];
		if (league_name.find("SSF") == std::string::npos) {
			leagues.push_back(league["id"]);
		}
	}

	return leagues;
}

std::vector<std::string> ApiHandler::GetStashTabList()
{
	auto data = http_.GetData("https://www.pathofexile.com/character-window/get-stash-items?accountName=" + user_.account_name_ + "&realm=pc&league=" + user_.selected_league_ + "&tabs=1&tabIndex=0");
	auto json = nlohmann::json::parse(data);

	std::vector<std::string> tab_list;
	for (const auto& tab : json["tabs"]) {
		tab_list.push_back(tab["n"]);
	}

	return tab_list;

}

std::vector<std::string> ApiHandler::GetStashItems(int index)
{
	auto data = http_.GetData("https://www.pathofexile.com/character-window/get-stash-items?accountName=" + user_.account_name_ + "&realm=pc&league=" + user_.selected_league_ + "&tabs=0&tabIndex=" + std::to_string(index));
	auto json = nlohmann::json::parse(data);

	std::vector<std::string> item_list;
	for (const auto& item : json["items"]) {
		if (item["identified"] == false) {
			continue;
		}
		if (item["name"] != "") {
			item_list.push_back(item["name"]);
		}
		else {
			item_list.push_back(item["typeLine"]);
		}
	}

	return item_list;
}

std::unordered_map<std::string, float> ApiHandler::GetPriceData(const std::string& league)
{
	std::string base_url = "https://poe.ninja/api/data/";
	std::array<std::string, 2> currencies = { "Currency", "Fragment" };
	std::array<std::string, 20> items = { "Watchstone", "Oil", "Incubator", 
											"Scarab", "Fossil", "Resonator", 
											"Essence", "DivinationCard", "Prophecy", 
											"SkillGem", "BaseType", "HelmetEnchant", 
											"UniqueMap", "Map", "UniqueJewel", 
											"UniqueFlask", "UniqueWeapon", "UniqueArmour", 
											"UniqueAccessory", "Beast" };

	std::unordered_map<std::string, float> price_data;

	if (!std::filesystem::exists("PriceData/" + league)) {
		std::filesystem::create_directories("PriceData/" + league);
	}
	auto league_encoded = league;
	if (league_encoded.find(" ") != std::string::npos) {
		league_encoded.replace(league.find(" "), 1, "%20");
	}

	for (const auto& currency : currencies) {

		std::ofstream file("PriceData/" + league + std::string("/") + currency + ".json");
		nlohmann::json json_file;


		auto data = http_.GetData(base_url + "currencyoverview?league=" + league_encoded + "&type=" + currency);
		auto json = nlohmann::json::parse(data);

		for (const auto& line : json["lines"]){
			std::string item = line["currencyTypeName"];
			float price = line["chaosEquivalent"];
			json_file[item] = price;
			
		}
		file << std::setw(4) << json_file << std::endl;
	}

	for (const auto& item : items) {
		auto data = http_.GetData(base_url + "itemoverview?league=" + league_encoded + "&type=" + item);
		auto json = nlohmann::json::parse(data);

		std::ofstream file("PriceData/" + league + std::string("/") + item + ".json");
		nlohmann::json json_file;
	
		for (const auto& line : json["lines"]) {
			std::string item = line["name"];
			float price = line["chaosValue"];
			json_file[item] = price;
		}

		file << std::setw(4) << json_file << std::endl;
	}
	return price_data;
}
