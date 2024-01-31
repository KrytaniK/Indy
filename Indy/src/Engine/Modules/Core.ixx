export module Indy_Core;

export import <memory>;
export import <iostream>;

// Global time tracking
export import Indy_Core_Time;

// Runtime behavior (Defined by CLIENT)
export import :Application;

// Logging, Profiling, etc.
export import :Logging;
export import :Profiling;

// Window handling
export import :Window;

// File operations (Read/Write)?
// export import :FS;

// Custom memory management (Potential Future Investment)?
// export import :Memory;

// Layer Stack
// export import :Layers;

// Rendering
// export import :Renderer;

export {
	namespace Indy
	{
		std::unique_ptr<Application> CreateApplication();
	}
}
