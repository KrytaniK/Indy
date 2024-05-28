module;

#include <cstdint>
#include <vector>
#include <memory>

export module Indy.Window:Manager;

import :Window;

export
{
	namespace Indy
	{

		class WindowManager
		{
		public:
			WindowManager();
			~WindowManager();

			void Update();

			Window* GetWindow(uint8_t index) const;
			Window* GetActiveWindow() const;

			Window* AddWindow(WindowCreateInfo& createInfo);

			void DestroyWindow(uint8_t index);

		private:
			uint8_t m_WindowCount = 0;
			std::vector<std::shared_ptr<Window>> m_Windows;
		};
	}
}