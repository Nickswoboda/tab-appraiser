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


	Application app(400,400);
	app.Run();
}