module;

export module Sandbox;

import Indy.Application;

import Indy.VulkanRenderer;

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