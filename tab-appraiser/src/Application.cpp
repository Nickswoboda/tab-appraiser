#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <json.hpp>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <filesystem>

Application::Application(int width, int height)
	:window_(width, height), api_handler_(user_)
{
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress))) {
		std::cout << "could not load GLAD";
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	Load();
	if (std::filesystem::exists("assets/fonts/Roboto-Medium.ttf")) {
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", font_size_);
	}

	ImGui_ImplGlfw_InitForOpenGL(window_.glfw_window_, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	SetImGuiStyle();
	
	glfwSetWindowUserPointer(window_.glfw_window_, this);
}

Application::~Application()
{
	Save();
	glfwTerminate();
}

void Application::Run()
{
	while (!glfwWindowShouldClose(window_.glfw_window_) && running_)
	{
		if (Window::IsFocused()) {
			Render();
		}

		glfwPollEvents();

		//Must change font outside of ImGui Rendering
		if (font_size_changed_) {
			font_size_changed_ = false;

			ImGuiIO& io = ImGui::GetIO();
			delete io.Fonts;
			io.Fonts = new ImFontAtlas();
			io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", font_size_);
			ImGui_ImplOpenGL3_CreateFontsTexture();
		}
	}

}

void Application::Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowSize({ (float)window_.width_, (float)window_.height_ });
	ImGui::Begin("Tab Appraiser", &running_, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
	ImGui::Separator();
	
	if (settings_open_) {
		RenderSettingsMenu();
	}
	else if (loading_price_data_) {
		LoadPriceData();
	}
	else {
		RenderAccount();

		if (!user_.account_name_.empty()) {
			RenderLeagues();

			if (!user_.selected_league_.empty()) {
				if (ninja_data_.empty()) {

					ImGui::PushStyleColor(ImGuiCol_Text, { 1.0, 0.0, 0.0, 1.0 });
					ImGui::Text("Unable to fetch price data");
					ImGui::PopStyleColor();
					if (ImGui::Button("Try Again")) {
						LoadPriceData();
					}
				}
				else {
					RenderStashTabs();

					if ((selected_stash_index_ != -1)) {
						RenderPriceInfo();
					}
				}
			}
		}
	}
	if (!settings_open_ && ImGui::Button("Settings")) {
		settings_open_ = true;
	}
	
	ImVec2 pos = ImGui::GetWindowPos();
	if (pos.x != 0.0f || pos.y != 0.0f) {
		ImGui::SetWindowPos({ 0.0f, 0.0f });
		window_.Move(pos.x, pos.y);
	}

	if (window_.height_ != ImGui::GetCursorPosY()) {
		window_.ResizeHeight(ImGui::GetCursorPosY());
	}
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window_.glfw_window_);
}

void Application::RenderAccount()
{
	ImGui::Text("Account: "); ImGui::SameLine(); ImGui::Text(user_.account_name_.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Change Account")) {
		changing_account_ = true;
	}

	if (changing_account_) {
		ImGui::Text("Please Enter Your POESESSID");

		static std::string error_text;
		if (!error_text.empty()) {
			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0, 0.0, 0.0, 1.0 });
			ImGui::Text(error_text.c_str());
			ImGui::PopStyleColor();
		}

		static char sess_id_input[100];
		ImGui::InputText("##Input", sess_id_input, 100);

		if (ImGui::Button("OK")) {
			SetPOESESSID(sess_id_input);

			if (user_.account_name_.empty()) {
				error_text = "Unable to validate POESESSID";
			}
			else {
				error_text.clear();
				user_.selected_league_.clear();
				changing_account_ = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			changing_account_ = false;
		}
	}
}

void Application::RenderLeagues()
{
	ImGui::Text("League: ");
	ImGui::SameLine();

	if (current_leagues_.empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0, 0.0, 0.0, 1.0 });
		ImGui::Text("Unable to get current league data.");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		if (ImGui::Button("Try Again")) {
			current_leagues_ = api_handler_.GetCurrentLeagues();
		}
	}
	else if (ImGui::BeginCombo("##LeagueCombo", user_.selected_league_.empty() ? "Select a League" : user_.selected_league_.c_str())) {
		for (auto& league : current_leagues_) {
			if (ImGui::Selectable(league.c_str())) {
				SetSelectedLeague(league);

				selected_stash_index_ = -1;
				stash_item_prices_.clear();

				user_.stash_tab_list_ = api_handler_.GetStashTabList();

				//don't waste time fetching price data for league that has no stash tabs
				if (!user_.stash_tab_list_.empty()) {
					loading_price_data_ = true;
				}
			}
		}
		//selectable popup does not close if user clicks out of window and loses focus
		//must do manually
		if (!Window::IsFocused()) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndCombo();
	}
}

void Application::RenderStashTabs()
{
	ImGui::Text("Stash Tab: ");
	ImGui::SameLine();

	if (user_.stash_tab_list_.empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0, 0.0, 0.0, 1.0 });
		ImGui::Text("Unable to find stash tabs.");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		if (ImGui::Button("Try Again")) {
			user_.stash_tab_list_ = api_handler_.GetStashTabList();
		}
	}
	else if (ImGui::BeginCombo("##StashTabsCombo", selected_stash_index_ == -1 ? "Select a Stash Tab" : user_.stash_tab_list_[selected_stash_index_].c_str())) {
		for (int i = 0; i < user_.stash_tab_list_.size(); i++) {
			if (ImGui::Selectable(user_.stash_tab_list_[i].c_str())) {
				selected_stash_index_ = i;
				stash_items_ = api_handler_.GetStashItems(selected_stash_index_);
				stash_item_prices_ = GetItemPrices();
			}
		}
		//selectable popup does not close if user clicks out of window and loses focus
		//must do manually
		if (!Window::IsFocused()) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndCombo();
	}
}

void Application::RenderPriceInfo()
{
	ImGui::Separator();
	if (!stash_item_prices_.empty()) {
		ImGui::BeginChildFrame(1, { (float)window_.width_, 250 });
		for (const auto& item : stash_item_prices_) {
			ImGui::Text(item.first.c_str());
			ImGui::SameLine(window_.width_ - 150);
			ImGui::Text("%.1f C", item.second);
		}
		ImGui::EndChildFrame();
	}
	else if (!ninja_data_.empty() && selected_stash_index_ != -1) {
		ImGui::Text("No price data available.");
	}
	ImGui::Separator();

	ImGui::Text("Price Threshold: ");
	ImGui::SameLine();
	if (ImGui::InputInt("##PriceThreshold", &price_threshold_)) {
		if (price_threshold_ < 0) {
			price_threshold_ = 0;
		}
		stash_item_prices_ = GetItemPrices();
	}

	if (!user_.selected_league_.empty() && !ninja_data_.empty()) {
		if (ImGui::Button("Update Price Data")) {
			loading_price_data_ = true;
		}
	}
}

void Application::RenderSettingsMenu()
{
	ImGui::TextWrapped("Font Size");
	ImGui::PushItemWidth(window_.width_);
	if (ImGui::DragInt("##font", &font_size_, 1.0f, 10, 32)) {
		font_size_changed_ = true;
	}
	ImGui::TextWrapped("Window Width");
	if (ImGui::DragInt("##width", &window_.width_, 1.0f, 400, 1000)) {
		window_.UpdateSize();
	}
	ImGui::PopItemWidth();

	if (ImGui::Button("Back")) {
		settings_open_ = false;
	}
}

void Application::LoadPriceData()
{
	static int iteration = 0;

	static std::array<const char*, 18> item_types = { "Currency", "Fragment",
											"Watchstone", "Oil", "Incubator",
											"Scarab", "Fossil", "Resonator",
											"Essence", "DivinationCard", "Prophecy",
											"UniqueMap", "Map", "UniqueJewel", 
											"UniqueFlask", "UniqueWeapon", "UniqueArmour", 
											"UniqueAccessory" };

	if (iteration < item_types.size()) {
		auto temp_data = api_handler_.GetPriceData(item_types[iteration]);
		ninja_data_.insert(temp_data.begin(), temp_data.end());
		ImGui::Text("Loading %s %s Data", user_.selected_league_.c_str(), item_types[iteration]);
		++iteration;
	}
	else {
		iteration = 0;
		loading_price_data_ = false;
		return;
	}

}

void Application::SetSelectedLeague(const std::string& league)
{
	user_.selected_league_ = league;

	api_handler_.league_encoded_ = league;
	if (league.find(" ") != std::string::npos) {
		api_handler_.league_encoded_.replace(league.find(" "), 1, "%20");
	}

	//reset price data;
	ninja_data_.clear();
}

void Application::Save()
{
	std::ofstream file("save-data.json");
	if (file.is_open()) {
		nlohmann::json json;
		json["POESESSID"] = user_.POESESSID_;
		json["selectedLeague"] = user_.selected_league_;
		json["windowX"] = window_.x_pos_;
		json["windowY"] = window_.y_pos_;
		json["windowWidth"] = window_.width_;
		json["fontSize"] = font_size_;

		file << std::setw(4) << json << std::endl;
	}
}

void Application::Load()
{
	current_leagues_ = api_handler_.GetCurrentLeagues();

	std::ifstream file("save-data.json");
	if (file.is_open()) {
		auto json = nlohmann::json::parse(file);

		if (json.count("POESESSID")) {
			user_.POESESSID_ = json["POESESSID"];
			if (!user_.POESESSID_.empty()) {
				SetPOESESSID(user_.POESESSID_.c_str());
			}
		}

		if (json.count("selectedLeague")) {
			SetSelectedLeague(json["selectedLeague"]);
			if (!user_.selected_league_.empty()) {
				
				//make sure loaded league is valid
				if (std::find(current_leagues_.begin(), current_leagues_.end(), user_.selected_league_) == current_leagues_.end()) {
					user_.selected_league_.clear();
				}
				else if (!user_.account_name_.empty()){
					user_.stash_tab_list_ = api_handler_.GetStashTabList();
					loading_price_data_ = true;
				}
			}
		}

		if (json.count("windowX") && json.count("windowY")) {
			window_.Move(json["windowX"], json["windowY"]);
		}
		if (json.count("windowWidth")) {
			window_.width_ = json["windowWidth"];
		}
		if (json.count("fontSize")) {
			font_size_ = json["fontSize"];
		}
	}
}

void Application::SetImGuiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(2, 2);

	style->WindowRounding = 0.0f;
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.2f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.9f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.9f);
}

void Application::SetPOESESSID(const char* id)
{
	user_.POESESSID_ = id;
	user_.account_name_ = api_handler_.GetAccountName();
}

std::vector<std::pair<std::string, float>> Application::GetItemPrices()
{
	std::vector<std::pair<std::string, float>> item_price;
	
	for (const auto& item : stash_items_) {
		if (ninja_data_.count(item) && (ninja_data_[item] >= price_threshold_)) {
			item_price.push_back({ item, ninja_data_[item] });
		}
	}

	std::sort(item_price.begin(), item_price.end(), [](auto& left, auto& right) {
		return left.second > right.second;
	});

	return item_price;
}





