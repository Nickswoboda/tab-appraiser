#pragma once

#include "Window.h"
#include "ApiHandler.h"

#include <imgui.h>
#include <string>

class Application
{
public:

	enum class State
	{
		Get_POESESSID,
		LeagueSelection
	};

	Application(int width, int height);
	~Application();

	void SetImGuiStyle();
	void SetPOESESSID(const char* id);

	void Run();
	void Render();

private:
	bool running_ = true;
	std::vector<std::string> current_leagues_;

	UserData user_;
	State state_;
	Window window_;
	ApiHandler api_handler_;
};