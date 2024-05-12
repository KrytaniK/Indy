module;

#include <cstdint>
#include <vector>
#include <memory>
#include <queue>

export module Indy.WindowManager;

export
{
	namespace Indy
	{
		class IWindow;
		struct IWindowHandle;
		struct WindowCreateInfo;

		class WindowManager
		{
		public:
			WindowManager();
			~WindowManager();

			void Update();

			IWindowHandle& GetWindow(uint8_t index);
			IWindowHandle& GetActiveWindow();

			IWindowHandle& AddWindow(WindowCreateInfo& createInfo);

			void DestroyWindow(uint8_t index);

		private:
			uint8_t m_WindowCount = 0;
			std::vector<std::shared_ptr<IWindow>> m_Windows;
			std::vector<IWindowHandle> m_Handles;
			std::queue<uint8_t> m_EmptyWindows;
		};
	}
}