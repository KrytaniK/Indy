module;

#include <memory>
#include <string>

export module Indy_Core;

// CLIENT derived application class
export import :Application;

// Debug Utilities
export import :Logging;
export import :Profiling;

// Window
import Indy_Core_Window;

// Input
import Indy_Core_InputSystem;

export {
	// ----- Utility Methods -----

	std::unique_ptr<Indy::ScopeProfiler> ProfileScope(const std::string& scope_sig);

	std::shared_ptr<Indy::ProfileSession> StartProfilingSession(const std::string& sessionName);

	void EndProfilingSession();

	namespace Indy
	{
		// Primary Application
		std::unique_ptr<Application> CreateApplication();

		// Method for creating a window.
		void OpenWindow(const WindowCreateInfo& createInfo = WindowCreateInfo());

		// Input Handling
		void ProcessInput();
	}
}
