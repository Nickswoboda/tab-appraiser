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

Application::Application(int width, int height)
	:window_(width, height), api_handler_(user_)
{
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress))) {
		std::cout << "could not load GLAD";
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(window_.glfw_window_, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	SetImGuiStyle();
	
	glfwSetWindowUserPointer(window_.glfw_window_, this);
	Load();
}

Application::~Application()
{
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
		//if (font_size_changed_) {
		//	font_size_changed_ = false;
		//
		//	ImGuiIO& io = ImGui::GetIO();
		//	delete io.Fonts;
		//	io.Fonts = new ImFontAtlas();
		//	io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", font_size_);
		//	ImGui_ImplOpenGL3_CreateFontsTexture();
		//}
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

	ImGui::Text("Account: "); ImGui::SameLine(); ImGui::Text(user_.account_name_.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Change Account")) {
		changing_account_ = true;
	}

	if (changing_account_) {
		ImGui::Text("Please Enter Your POESESSID");
		static char sess_id_input[100];
		ImGui::InputText("##Input", sess_id_input, 100);

		if (ImGui::Button("OK")) {
			SetPOESESSID(sess_id_input);
			changing_account_ = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			changing_account_ = false;
		}
	}

	if (!user_.account_name_.empty() && user_.account_name_ != "error") {
		ImGui::Text("League: ");
		ImGui::SameLine();

		static std::string combo_selection = user_.selected_league_.empty() ? "Select a League" : user_.selected_league_;
		if (ImGui::BeginCombo("##LeagueCombo", combo_selection.c_str())) {
			for (auto& league : current_leagues_) {
				if (ImGui::Selectable(league.c_str())) {
					combo_selection = league;
					user_.selected_league_ = league;
					user_.stash_tab_list_ = api_handler_.GetStashTabList();
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

	if (!user_.selected_league_.empty()) {
		ImGui::Text("Stash Tab: ");
		ImGui::SameLine();
		static std::string combo_selection = "Select a Stash Tab";
		if (ImGui::BeginCombo("##StashTabsCombo", combo_selection.c_str())) {
			for (int i = 0; i < user_.stash_tab_list_.size(); i++) {
				if (ImGui::Selectable(user_.stash_tab_list_[i].c_str())) {
					combo_selection = user_.stash_tab_list_[i];
					selected_stash_index_ = i;
					stash_items_ = api_handler_.GetStashItems(selected_stash_index_);
					if (ninja_data_.empty()) {
						ninja_data_ = api_handler_.GetPriceData(user_.selected_league_);
					}
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

	ImGui::Separator();

	if (!stash_item_prices_.empty()) {
		ImGui::BeginChildFrame(1, { (float)window_.width_, 300 });
		for (const auto& item : stash_item_prices_) {
			ImGui::Text(item.first.c_str());
			ImGui::SameLine(200);
			ImGui::Text("%.1f", item.second);
		}
		ImGui::EndChildFrame();
	}
	
	ImGui::Text("Price Threshold: ");
	ImGui::SameLine();
	if (ImGui::InputInt("##PriceThreshold", &price_threshold_)) {
		stash_item_prices_ = GetItemPrices();
	}

	if (!user_.selected_league_.empty()) {
		if (ImGui::Button("Update Price Info")) {
			 ninja_data_= api_handler_.GetPriceData(user_.selected_league_);
			 stash_item_prices_ = GetItemPrices();
		}
	}

	if (ImGui::Button("Save")) {
		Save();
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

void Application::Save()
{
	std::ofstream file("save-data.json");

	nlohmann::json json;
	json["POESESSID"] = user_.POESESSID_;
	json["selectedLeague"] = user_.selected_league_;
	json["windowX"] = window_.x_pos_;
	json["windowY"] = window_.y_pos_;
	
	file << std::setw(4) << json << std::endl;
}

void Application::Load()
{
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
			user_.selected_league_ = json["selectedLeague"];
			if (!user_.selected_league_.empty()) {
				user_.stash_tab_list_ = api_handler_.GetStashTabList();
			}
		}

		if (json.count("windowX") && json.count("windowY")) {
			window_.Move(json["windowX"], json["windowY"]);
		}
	}

	current_leagues_ = api_handler_.GetCurrentLeagues();
}

void Application::SetImGuiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(2, 2);

	style->WindowRounding = 0.0f;
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.2f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
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
		if (ninja_data_.count(item)) {
			if (ninja_data_[item] > price_threshold_) {
				item_price.push_back({ item, ninja_data_[item] });
			}
		}
	}

	std::sort(item_price.begin(), item_price.end(), [](auto& left, auto& right) {
		return left.second > right.second;
	});

	return item_price;
}




