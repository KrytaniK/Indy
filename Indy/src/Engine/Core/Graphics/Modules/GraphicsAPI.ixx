module;

#include <cstdint>
#include <memory>

export module Indy.Graphics:GraphicsAPI;

import Indy.Window;

export
{
	namespace Indy
	{
		class GraphicsAPI
		{
		public:
			enum Type : uint8_t { None = 0x00, Vulkan };

			static std::unique_ptr<GraphicsAPI> Create(const GraphicsAPI::Type& api);

		public:
			GraphicsAPI();
			virtual ~GraphicsAPI() = default;

			virtual void OnLoad() = 0;
			virtual void OnStart() = 0;
			virtual void OnUnload() = 0;

			virtual void OnWindowDispatch(WindowDispatchEvent* event) = 0;
		};
	}
}