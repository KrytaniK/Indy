#pragma once

#include <iostream>
#include <memory>

namespace Engine
{
	struct WindowSpec
	{
		std::string title;
		unsigned int width, height;

		WindowSpec(const std::string& Title = "Indy Engine", unsigned int Width = 1080, unsigned int Height = 720)
			: title(Title), width(Width), height(Height) {};
	};

	// Abstract representation of a desktop window | To be inherited for platform specific window creation
	class  Window
	{
	public:
		virtual ~Window() = default;

		virtual void onUpdate() = 0;
		virtual void* GetWindow() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		static std::unique_ptr<Window> Create(const WindowSpec& spec = WindowSpec());
	};
}