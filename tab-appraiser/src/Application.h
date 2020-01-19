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
	void Render();

private:
	bool running_ = true;

	Window window_;
};