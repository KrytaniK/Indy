module;

#include <memory>
#include <string>

export module Indy_Core:Window;

export
{
	namespace Indy
	{
		struct WindowCreateInfo
		{
			std::string title = "Indy Engine";
			unsigned int width = 1280, height = 760;
		};

		struct WindowProps
		{
			std::string title;
			unsigned int width, height;
			bool minimized, focused, vSyncEnabled;
		};

		// Method for creating a window.
		void OpenWindow(const WindowCreateInfo& createInfo = WindowCreateInfo());

		// Abstract interface for platform-specific window implementation
		class IWindow
		{
		public:
			virtual ~IWindow() = default;

			virtual int Width() = 0;
			virtual int Height() = 0;

			virtual void* NativeWindow() const = 0;
		};

		// Windows OS Window Implementation
		/*class WindowsWindow : public IWindow
		{
		public:
			WindowsWindow(const WindowCreateInfo& createInfo);
			virtual ~WindowsWindow();

			virtual int Width() const override {};
			virtual int Height() const override {};

			virtual void* NativeWindow() const override;

		private:
			WindowProps m_Props;
		};*/


		//// Mac OS Window Implementation
		//class MacWindow : public IWindow
		//{
		//public:
		//	MacWindow(const WindowCreateInfo& createInfo);
		//	virtual ~MacWindow();

		//	unsigned int Width() const override;
		//	unsigned int Height() const override;

		//	virtual void* NativeWindow() const override;

		//private:
		//	WindowProps m_Props;
		//};


		//// Linux OS Window Implementation
		//class LinuxWindow : public IWindow
		//{
		//public:
		//	LinuxWindow(const WindowCreateInfo& createInfo);
		//	virtual ~LinuxWindow();

		//	unsigned int Width() const override;
		//	unsigned int Height() const override;

		//	virtual void* NativeWindow() const override;

		//private:
		//	WindowProps m_Props;
		//};
	}
}