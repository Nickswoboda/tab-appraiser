#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <iostream>

Application::Application(int width, int height)
	:window_(width, height), api_handler_(user_)
{
	api_handler_.http_.SetVerbose(true);
	current_leagues_ = api_handler_.GetCurrentLeagues();
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
	ImGui::Begin("Window");

	switch (state_) {
		case State::Get_POESESSID:
			ImGui::Text("Please Enter Your POESESSID");
			static char sess_id_input[100];
			ImGui::InputText("##Input", sess_id_input, 100);
			
			if (ImGui::Button("OK")){
				SetPOESESSID(sess_id_input);
				state_ = State::LeagueSelection;
			}
			break;

		case State::LeagueSelection:
		{
			ImGui::Text(user_.account_name_.c_str());
			ImGui::Text("Select a league");

			static auto combo_selection = current_leagues_[0];
			if (ImGui::BeginCombo("Leagues: ", combo_selection.c_str())) {
				for (auto& league : current_leagues_) {
					if (ImGui::Selectable(league.c_str())) {
						combo_selection = league;
					}
				}
				//selectable popup does not close if user clicks out of window and loses focus
				//must do manually
				if (!Window::IsFocused()) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Ok")) {
				user_.selected_league_ = combo_selection;
				user_.stash_tab_list_ = api_handler_.GetStashTabList();
				state_ = State::StashTabList;
			}
			break;
		}
		case State::StashTabList:
		{
			ImGui::Text(user_.account_name_.c_str());
			ImGui::Text(user_.selected_league_.c_str());

			static auto combo_selection = user_.stash_tab_list_[0];
			static int selection_index = 0;
			if (ImGui::BeginCombo("Tabs: ", combo_selection.c_str())) {
				for (int i = 0; i < user_.stash_tab_list_.size(); i++) {
					if (ImGui::Selectable(user_.stash_tab_list_[i].c_str())) {
						combo_selection = user_.stash_tab_list_[i];
						selection_index = i;
					}
				}
				//selectable popup does not close if user clicks out of window and loses focus
				//must do manually
				if (!Window::IsFocused()) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Ok")) {
				selected_stash_index_ = selection_index;
				stash_items_ = api_handler_.GetStashItems(selected_stash_index_);
				price_data_ = GetItemPrices();
				state_ = State::ItemList;
			}
			break;
		}
		case State::ItemList:
		{
			ImGui::Text(user_.account_name_.c_str());
			ImGui::Text(user_.selected_league_.c_str());
			ImGui::Text(user_.stash_tab_list_[selected_stash_index_].c_str());

			for (const auto& item : price_data_) {
				ImGui::Text(item.first.c_str());
				ImGui::SameLine();
				ImGui::Text(std::to_string(item.second).c_str());
			}

			if (ImGui::Button("Back")) {
				state_ = State::StashTabList;
			}
			break;
		}
			
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
	
	std::cout << user_.account_name_ << "\n";

	std::cout << "Current Leagues: ";
	for (const auto& league : current_leagues_) {
		std::cout << league << ", ";
	}
	std::cout << "\n";
	
}

std::unordered_map<std::string, float> Application::GetItemPrices()
{
	auto price_data = api_handler_.GetPriceData();
	std::unordered_map<std::string, float> item_price;

	for (const auto& item : stash_items_) {
		if (price_data.count(item)) {
			item_price[item] = price_data[item];
		}
	}

	return item_price;
}




