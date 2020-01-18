#include "Http.h"
#include "Application.h"

#include <iostream>

//currencyoverview
//-Currency
//-Fragments

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
int main()
{

	
	PoeNinjaQuery query;
	query.overview = "item";
	query.league = "Metamorph";
	query.itemtype = "Prophecy";
	Http http;
	//auto result = http.GetJson(query.Get());
	//
	//if (query.overview == "currency")
	//	for (auto& currency : result["lines"]) {
	//		std::cout << currency["currencyTypeName"] << "  ";
	//		std::cout << currency["chaosEquivalent"] << "\n";
	//	}
	//else {
	//	for (auto& currency : result["lines"]) {
	//		std::cout << currency["name"] << "  ";
	//		std::cout << currency["chaosValue"] << "\n";
	//	}
	//}

	auto result = http.GetString("https://www.pathofexile.com/my-account");
	std::cout << result;


	Application app(400,400);
	app.Run();
}