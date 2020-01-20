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




