export module Indy_Core_LayerStack:Layer;

export
{
	namespace Indy
	{
		class Layer
		{
		public:
			virtual void onAttach() = 0;
			virtual void onDetach() = 0;
			virtual void Update() = 0;
		};
	}
}