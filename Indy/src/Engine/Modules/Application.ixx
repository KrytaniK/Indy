export module Core:Application;

export {
	namespace Indy
	{
		class Application
		{
		public:
			Application();
			virtual ~Application();
			virtual void Run() = 0;

			void Terminate();

		protected:
			bool m_Terminate;
			bool m_Minimized;
		};
	}
}