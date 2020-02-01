#pragma once

#include "Window.h"
#include "ApiHandler.h"

#include <imgui.h>
#include <string>
#include <unordered_map>
#include <stack>

class Application
{
public:

	enum class State 
	{
		GetLeagueData,
		GetPriceData,
		GetStashItems,
		Error,
		Render,
	};

	Application(int width, int height);
	~Application();

	void SetImGuiStyle();
	void SetPOESESSID(const char* id);
	std::vector<std::pair<std::string, float>> GetItemPrices();

	void Run();
	void Render();
	void RenderAccount();
	void RenderLeagues();
	void RenderStashTabs();
	void RenderPriceInfo();
	void RenderSettingsMenu();
	void LoadPriceData();
	void SetSelectedLeague(const std::string& league);

	void Save();
	void Load();

private:
	bool running_ = true;
	bool changing_account_ = false;
	std::vector<std::string> current_leagues_;
	std::vector<std::string> stash_items_;
	std::vector<std::pair<std::string, float>> stash_item_prices_;
	std::unordered_map<std::string, float> ninja_data_;
	int selected_stash_index_= -1;
	int price_threshold_ = 0;
	bool loading_price_data_ = false;
	int font_size_ = 16;
	bool font_size_changed_ = false;
	bool settings_open_ = false;

	UserData user_;
	Window window_;
	ApiHandler api_handler_;
};