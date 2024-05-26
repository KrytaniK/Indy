module;

#include <cstdint>
#include <memory>

export module Indy.Graphics:IRenderAPI;

export
{
	namespace Indy::Graphics
	{
		enum class RenderAPI : uint8_t { None = 0x00, Vulkan };

		class IRenderAPI
		{
		public:
			IRenderAPI();
			virtual ~IRenderAPI() = default;

			virtual void OnLoad() = 0;
			virtual void OnStart() = 0;
			virtual void OnUnload() = 0;
		};

		std::unique_ptr<IRenderAPI> CreateRenderAPI(const RenderAPI& api);
	}
}