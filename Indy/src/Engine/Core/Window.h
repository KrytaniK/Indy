#pragma once

#include "Core.h"
#include <string>

namespace Engine
{
	struct WindowSpec
	{
		std::string title;
		unsigned int width, height;
		bool b_Minimized, b_Maximized, b_VSyncEnabled;

		WindowSpec(const std::string& Title = "Indy Engine", unsigned int Width = 1280, unsigned int Height = 720)
			: title(Title), width(Width), height(Height), b_Minimized(false), b_Maximized(false), b_VSyncEnabled(true) {};
	};

	// Abstract representation of a desktop window | To be inherited for platform specific window creation
	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void onUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

	private:
		Window() {}; // Disable Default Constructor

		virtual Window* Create(WindowSpec& spec = WindowSpec()) = 0;
	};
}