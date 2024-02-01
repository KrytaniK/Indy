module;

#include <cstdint>
#include <vector>
#include <memory>

export module Indy_Core_LayerStack;

export import :Layer;
export import :WindowLayer;

export
{
	namespace Indy
	{
		class LayerStack
		{
		public:
			static std::vector<Layer*> s_Layers;

			static void Cleanup();

			static void Update();

			static void Push(Layer* layer);
			static void PushOverlay(Layer* layer);

		private:
			static uint32_t s_LayerIndex;
		};
	}
}