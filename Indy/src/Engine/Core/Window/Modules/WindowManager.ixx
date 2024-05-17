module;

#include <cstdint>
#include <vector>
#include <memory>

export module Indy.Window:Manager;

import :IWindow;

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

			const IWindow* GetWindow(uint8_t index) const;
			const IWindow* GetActiveWindow() const;

			void AddWindow(WindowCreateInfo& createInfo);

			void DestroyWindow(uint8_t index);

		private:
			uint8_t m_WindowCount = 0;
			std::vector<std::shared_ptr<IWindow>> m_Windows;
		};
	}
}