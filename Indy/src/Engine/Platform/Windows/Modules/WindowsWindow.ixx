module;

#include <GLFW/glfw3.h>

export module Indy.WindowsWindow;

import Indy.Window;
import Indy.Input;

export
{
	namespace Indy
	{
		// Windows OS Window Implementation
		class WindowsWindow : public IWindow
		{
		public:
			WindowsWindow(const WindowCreateInfo& createInfo);
			virtual ~WindowsWindow();

			virtual void Update() override;

			virtual void* NativeWindow() const override;
			virtual const WindowProps& Properties() const override;

			virtual Input::InputContext* GetInputContext() override;

			virtual void SetExtent(const int& width, const int& height);
			virtual void SetFocus(bool isFocused);
			virtual void SetMinimized(bool isMinimized);


		private:
			void SetGLFWWindowCallbacks();

		private:
			WindowProps m_Props;
			GLFWwindow* m_NativeWindow;
		};
	}
}