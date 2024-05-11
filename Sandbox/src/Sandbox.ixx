module;

export module Sandbox;

export import Indy_Core;

import Indy_Core_Renderer;

export {
	namespace Indy
	{
		class Sandbox : public Application
		{
		public:
			Sandbox();
			~Sandbox();

			virtual void Run() override;

		private:
			std::unique_ptr<VulkanRenderer> m_Renderer;
		};
	}
}