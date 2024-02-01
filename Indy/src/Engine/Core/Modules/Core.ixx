module;

#include <memory>

export module Indy_Core;

export import :Application; // Runtime Behavior

// Debug Utilities
export import :Logging;
export import :Profiling;

import Indy_Core_Window;

export {
	namespace Indy
	{
		// Primary Application
		std::unique_ptr<Application> CreateApplication();

		// Method for creating a window.
		void OpenWindow(const WindowCreateInfo& createInfo = WindowCreateInfo());

		// ----- Utility Methods -----

		std::unique_ptr<Indy::ScopeProfiler> ProfileScope(const std::string& scope_sig);

		std::shared_ptr<Indy::ProfileSession> StartProfilingSession(const std::string& sessionName);

		void EndProfilingSession();
	}
}
