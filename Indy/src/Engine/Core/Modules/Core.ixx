module;

#include <memory>

// Core Module should only be used in CLIENT projects.
export module Indy_Core;

// CLIENT derived application class
export import :Application;

// Debug Utilities
export import :Logging;

//// Window
//import Indy_Core_Window;
//
//// Input
//import Indy_Core_InputSystem;

export {
	namespace Indy
	{
		// MUST be overridden by CLIENT to define application-specific behavior
		std::unique_ptr<Application> CreateApplication();
	}
}