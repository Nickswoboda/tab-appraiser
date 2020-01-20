#include "Http.h"
#include "Application.h"

#include <iostream>
#include <vector>

class PoeNinjaQuery 
{
public:
	std::string url = "https://poe.ninja/api/data/";
	std::string overview;
	std::string league;
	std::string itemtype;

	std::string Get() 
	{
		return url + overview + "overview?league=" + league + "&type=" + itemtype;
	}
};

std::vector<std::string> GetStashTabList(nlohmann::json& data)
{
	std::vector<std::string> tab_list;
	for (const auto& tab : data) {
		tab_list.push_back(tab["n"]);
	}

	return tab_list;
}

std::vector<std::string> GetItemList(nlohmann::json& data)
{
	std::vector<std::string> item_list;
	for (const auto& item : data) {
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

int main()
{

	PoeNinjaQuery query;
	query.overview = "item";
	query.league = "Metamorph";
	query.itemtype = "Prophecy";
	Http http;
	http.SetCookie("");
	auto result = http.GetData(query.Get());
	auto json = nlohmann::json::parse(result);
	
	if (query.overview == "currency")
		for (auto& currency : json["lines"]) {
			std::cout << currency["currencyTypeName"] << "  ";
			std::cout << currency["chaosEquivalent"] << "\n";
		}
	else {
		for (auto& currency : json["lines"]) {
			std::cout << currency["name"] << "  ";
			std::cout << currency["chaosValue"] << "\n";
		}
	}

	//auto stash_tab_info = http.GetJson("https://www.pathofexile.com/character-window/get-stash-items?accountName=" + user_data.account_name_ + "&realm=pc&league=Hardcore%20Metamorph&tabs=1&tabIndex=0");
	//auto stash_tab_list = GetStashTabList(stash_tab_info["tabs"]);
	//
	//for (const auto& tab : stash_tab_list) {
	//	std::cout << tab << ", ";
	//}
	//std::cout << "\n";
	//
	//auto item_info = http.GetJson("https://www.pathofexile.com/character-window/get-stash-items?accountName=" + user_data.account_name_ + "&realm=pc&league=Hardcore%20Metamorph&tabs=0&tabIndex=27");
	//auto item_list = GetItemList(item_info["items"]);
	//
	//for (const auto& item : item_list) {
	//	std::cout << item << "\n";
	//}

	Application app(400,400);
	app.Run();
}