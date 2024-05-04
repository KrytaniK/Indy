module;

#include <vector>
#include <memory>

export module Indy_Core_LayerStack;

export import :Layer;
//export import :WindowLayer;

export
{
	namespace Indy
	{
		class LayerStack
		{
		public:
			LayerStack();
			~LayerStack();

			void Update();
			void PushLayer(const std::shared_ptr<ILayer> layer);
			void PushOverlay(const std::shared_ptr<ILayer> overlay); // should likely derive an Overlay class from Layer

		private:
			std::vector<std::shared_ptr<ILayer>> m_Layers;
			uint32_t m_OverlayStartIndex;
		};
	}
}