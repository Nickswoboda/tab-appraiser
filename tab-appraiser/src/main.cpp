#include "Http.h"
#include "Application.h"

#include <iostream>
#include <vector>

//currencyoverview
//-Currency
//-Fragments

struct AccountData 
{
	std::string account_name_;
	std::vector<std::string> leagues_;
};
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

std::string GetAccountName(std::string& data)
{
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
	AccountData accnt_data;
	auto result = http.GetString("https://www.pathofexile.com/my-account");
	accnt_data.account_name_ = GetAccountName(result);
	
	auto league_info = http.GetJson("http://api.pathofexile.com/leagues");
	for (const auto& league : league_info) {
		accnt_data.leagues_.push_back(league["id"]);
	}

	std::cout << "Account Name: " << accnt_data.account_name_ << "\n";
	std::cout << "Current Leagues: ";
	for (const auto& league : accnt_data.leagues_) {
		std::cout << league << ", ";
	}

	Application app(400,400);
	app.Run();
}