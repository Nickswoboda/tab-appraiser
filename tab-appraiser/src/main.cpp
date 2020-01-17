#include "Http.h"
#include "Application.h"

#include <iostream>

int main()
{
	Http http;
	auto result = http.Get("https://poe.ninja/api/data/currencyoverview?league=Metamorph&type=Currency");

	for (auto& currency : result["lines"]) {
		std::cout << currency["currencyTypeName"] << "  ";
		std::cout << currency["chaosEquivalent"] << "\n";
	}

	Application app(400,400);
	app.Run();
}