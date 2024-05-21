export module Sandbox;

import Indy.Application;
import Indy.Input;

export {
	namespace Indy
	{
		class Sandbox : public Application
		{
		public:
			Sandbox(const ApplicationCreateInfo& createInfo);
			virtual ~Sandbox() override;

			virtual void OnLoad() override;
			virtual void OnStart() override;
			virtual void OnUpdate() override;
			virtual void OnUnload() override;

		private:
			Input::InputContext m_InputContext;
		};
	}
}