#define NOMINMAX
#define RAPIDJSON_NOMEMBERITERATORCLASS

#include "ApiHandler.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <fstream>
#include <iomanip>
#include <filesystem>
#include <array>

ApiHandler::ApiHandler(UserData& user)
	:user_(user)
{
	poe_curl_handle_ = http_.CreateHandle();
	ninja_curl_handle_ = http_.CreateHandle();

	http_.SetVerbose(poe_curl_handle_, true);
	http_.SetVerbose(ninja_curl_handle_, true);
}

void ApiHandler::SetPOESESSIDCookie()
{
	http_.SetCookie(poe_curl_handle_, "POESESSID=" + user_.POESESSID_);
}

std::string ApiHandler::GetAccountName()
{
	SetPOESESSIDCookie();

	auto data = http_.GetData(poe_curl_handle_, "https://www.pathofexile.com/my-account");
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
	return "error";
}

std::vector<std::string> ApiHandler::GetCurrentLeagues()
{
	auto data = http_.GetData(poe_curl_handle_, "http://api.pathofexile.com/leagues");
	rapidjson::Document json;
	json.ParseInsitu(data.data());

	std::vector<std::string> leagues;
	for (const auto& league : json.GetArray()) {
		std::string league_name = league["id"].GetString();
		if (league_name.find("SSF") == std::string::npos) {
			leagues.push_back(league_name);
		}
	}

	return leagues;
}

std::vector<std::string> ApiHandler::GetStashTabList()
{
	auto data = http_.GetData(poe_curl_handle_, 
		"https://www.pathofexile.com/character-window/get-stash-items?accountName=" + user_.account_name_ + "&realm=pc&league=" + user_.selected_league_ + "&tabs=1&tabIndex=0");
	rapidjson::Document json;
	json.ParseInsitu(data.data());

	std::vector<std::string> tab_list;
	for (const auto& tab : json["tabs"].GetArray()) {
		tab_list.push_back(tab["n"].GetString());
	}

	return tab_list;

}

std::vector<std::string> ApiHandler::GetStashItems(int index)
{
	auto data = http_.GetData(poe_curl_handle_, 
		"https://www.pathofexile.com/character-window/get-stash-items?accountName=" + user_.account_name_ + "&realm=pc&league=" + user_.selected_league_ + "&tabs=0&tabIndex=" + std::to_string(index));
	rapidjson::Document json;
	json.ParseInsitu(data.data());

	std::vector<std::string> item_list;
	for (const auto& item : json["items"].GetArray()) {
		if (item["identified"] == false) {
			continue;
		}
		if (item["name"] != "") {
			item_list.push_back(item["name"].GetString());
		}
		else {
			item_list.push_back(item["typeLine"].GetString());
		}
	}

	return item_list;
}

std::unordered_map<std::string, float> ApiHandler::GetPriceData(const std::string& league)
{
	std::string base_url = "https://poe.ninja/api/data/";
	std::array<std::string, 2> currencies = { "Currency", "Fragment" };
	std::array<std::string, 17> items = { "Watchstone", "Oil", "Incubator", 
											"Scarab", "Fossil", "Resonator", 
											"Essence", "DivinationCard", "Prophecy", 
											"SkillGem", "UniqueMap", "Map", 
											"UniqueJewel", "UniqueFlask", "UniqueWeapon", 
											"UniqueArmour", "UniqueAccessory"};

	std::unordered_map<std::string, float> price_info;

	if (!std::filesystem::exists("PriceData/" + league)) {
		std::filesystem::create_directories("PriceData/" + league);
	}
	auto league_encoded = league;
	if (league_encoded.find(" ") != std::string::npos) {
		league_encoded.replace(league.find(" "), 1, "%20");
	}

	for (const auto& currency : currencies) {

		auto data = http_.GetData(ninja_curl_handle_, base_url + "currencyoverview?league=" + league_encoded + "&type=" + currency);
		rapidjson::Document json;
		json.ParseInsitu(data.data());

		for (const auto& line : json["lines"].GetArray()){
			price_info[line["currencyTypeName"].GetString()] = line["chaosEquivalent"].GetFloat();
		}
	}

	for (const auto& item : items) {
		auto data = http_.GetData(ninja_curl_handle_, base_url + "itemoverview?league=" + league_encoded + "&type=" + item);
		rapidjson::Document json;
		json.ParseInsitu(data.data());
		
		rapidjson::Document price_data;
		price_data.SetObject();
		for (const auto& line : json["lines"].GetArray()) {
			price_info[line["name"].GetString()] = line["chaosValue"].GetFloat();
		}
	
	}
	return price_info;
}
