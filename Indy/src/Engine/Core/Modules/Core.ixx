module;

#include <memory>

// Core Module should only be used in CLIENT projects.
export module Indy_Core;

// CLIENT derived application class
export import :Application;

// Default Layer Stack for layered architectures
export import :LayerStack;

// Debug Utilities
export import Indy_Core_Debug;
export import Indy_Core_Profiler;

export import :WindowLayer;
export import :InputLayer;

export {
	namespace Indy
	{
		// MUST be overridden by CLIENT to define application-specific behavior
		std::unique_ptr<Application> CreateApplication();
	}
}