export module Sandbox;

export import Indy.Application;

export {
	namespace Indy
	{
		class Sandbox : public Application
		{
		public:
			Sandbox(const ApplicationCreateInfo& createInfo);
			virtual ~Sandbox() override;

			virtual void Load() override;
			virtual void Start() override;
			virtual void Update() override;
			virtual void Shutdown() override;
			virtual void Unload() override;
		};
	}
}