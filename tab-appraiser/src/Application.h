#pragma once

#include "Window.h"

#include <imgui.h>
#include <string>

class Application
{
public:

	Application(int width, int height);
	~Application();

	void SetImGuiStyle();

	void Run();

private:
	bool running_ = true;

	std::string error_message_;

	int font_size_= 16;
	bool font_size_changed_ = false;

	bool all_ears_enabled_ = true;
	bool no_stone_unturned_enabled_ = true;
	int tutorial_page_ = 1;

	Window window_;
};