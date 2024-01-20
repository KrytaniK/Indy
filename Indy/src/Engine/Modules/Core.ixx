export module Core;

export import <memory>;
export import <iostream>;

// Runtime behavior (Defined by CLIENT)
export import :Application;

// Logging, Profiling, etc.
// export import :Debug;

// File operations (Read/Write)
// export import :FS;

// Custom memory management (Potential Future Investment)
// export import :Memory;

// Layer Stack
// export import :Layers;

// Window handling
// export import :Window;

// Rendering
// export import :Renderer;

export {
	namespace Indy
	{
		std::unique_ptr<Application> CreateApplication();
	}
}
