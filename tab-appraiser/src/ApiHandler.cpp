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
#include <iostream>

ApiHandler::ApiHandler(UserData& user, std::stack<Error>& errors)
	:user_(user), errors_(errors)
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

	std::string accnt_name;
	auto temp_pos = data.find(temp);
	if (temp_pos != std::string::npos) {
		auto substring = data.substr(temp_pos + temp_length, 100);

		for (auto& character : substring) {
			if (character == '\"') {
				break;
			}
			accnt_name += character;
		}
	}

	if (accnt_name.empty()) {
		errors_.push(Error::AccountError);
	}
	return accnt_name;
}

std::vector<std::string> ApiHandler::GetCurrentLeagues()
{
	auto data = http_.GetData(poe_curl_handle_, "http://api.pathofexile.com/leagues");
	rapidjson::Document json;
	json.ParseInsitu(data.data());

	std::vector<std::string> leagues;

	if (json.HasParseError()) {
		std::cout << "Unable to parse current league data \n";
	}
	else {
		for (const auto& league : json.GetArray()) {
			std::string league_name = league["id"].GetString();
			if (league_name.find("SSF") == std::string::npos) {
				leagues.push_back(league_name);
			}
		}
	}

	if (leagues.empty()) {
		errors_.push(Error::LeagueError);
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
	if (json.HasParseError()) {
		std::cout << "Unable to parse stash tab list \n";
		errors_.push(Error::StashListError);
	}
	else {
		if (json.HasMember("tabs")) {
			for (const auto& tab : json["tabs"].GetArray()) {
				tab_list.push_back(tab["n"].GetString());
			}
		}
	}

	if (tab_list.empty()) {
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
	if (json.HasParseError()) {
		std::cout << "Unable to parse stash items\n";
		errors_.push(Error::StashItemsError);
	}
	else {
		if (json.HasMember("items")) {
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
		}
	}

	return item_list;
}

std::unordered_map<std::string, float> ApiHandler::GetPriceData(const std::string& league)
{
	static std::string base_url = "https://poe.ninja/api/data/";
	static std::array<std::string, 2> currencies = { "Currency", "Fragment" };
	static std::array<std::string, 17> items = { "Watchstone", "Oil", "Incubator", 
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

		if (json.HasParseError()) {
			std::cout << "Unable to parse " << league << " " << currency << " price data \n";
			continue;
		}

		for (const auto& line : json["lines"].GetArray()){
			price_info[line["currencyTypeName"].GetString()] = line["chaosEquivalent"].GetFloat();
		}
	}

	for (const auto& item : items) {
		auto data = http_.GetData(ninja_curl_handle_, base_url + "itemoverview?league=" + league_encoded + "&type=" + item);
		rapidjson::Document json;
		json.ParseInsitu(data.data());
		
		if (json.HasParseError()) {
			std::cout << "Unable to parse " << league << " " << item << " price data \n";
			continue;
		}

		for (const auto& line : json["lines"].GetArray()) {
			price_info[line["name"].GetString()] = line["chaosValue"].GetFloat();
		}
	
	}

	if (price_info.empty()) {
		errors_.push(Error::StashListError);
	}
	return price_info;
}
