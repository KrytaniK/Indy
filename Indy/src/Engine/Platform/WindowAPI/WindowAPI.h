#pragma once

#include "Engine/Core/Window.h"

#include <memory>

namespace Engine
{
	class WindowAPI
	{
	public:
		enum class Type
		{
			None = 0, GLFW
		};


	private:
		static Type s_Type;

	public:
		virtual ~WindowAPI() = default;

		virtual void onUpdate() = 0;
		virtual void* GetWindow() = 0;

	public:
		static Type GetType() { return s_Type; };
		static void Set(Type type) { 
			if (s_Type != Type::None) // Type shouldn't change at runtime.
				return;

			s_Type = type;
		};

		static std::unique_ptr<WindowAPI> Create(const WindowSpec& windowSpec);
	};
}

#define WINDOW_API_GLFW Engine::WindowAPI::Type::GLFW