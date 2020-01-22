#pragma once

#include "Window.h"
#include "ApiHandler.h"

#include <imgui.h>
#include <string>
#include <unordered_map>

class Application
{
public:

	enum class State
	{
		Get_POESESSID,
		LeagueSelection,
		StashTabList,
		ItemList
	};

	Application(int width, int height);
	~Application();

	void SetImGuiStyle();
	void SetPOESESSID(const char* id);
	std::unordered_map<std::string, float> GetItemPrices();

	void Run();
	void Render();

private:
	bool running_ = true;
	std::vector<std::string> current_leagues_;
	std::vector<std::string> stash_items_;
	std::unordered_map<std::string, float> price_data_;
	int selected_stash_index_;

	UserData user_;
	State state_;
	Window window_;
	ApiHandler api_handler_;
};