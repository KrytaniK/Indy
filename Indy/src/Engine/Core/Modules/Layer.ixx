export module Indy_Core:Layer;

export
{
	namespace Indy
	{
		class ILayer
		{
		public:
			virtual void onAttach() = 0;
			virtual void onDetach() = 0;
			virtual void Update() = 0;
		};
	}
}