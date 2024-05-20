module;

#include <string>
#include <memory>
#include <unordered_map>

export module Indy.Input:DeviceManager;

import :DeviceBuilder;
import :Device;
import :Control;

export
{
	namespace Indy::Input
	{
		class DeviceManager
		{
		public:
			DeviceManager();
			~DeviceManager() = default;

			void AddLayout(const Layout& layout);

			void AddDevice(const DeviceInfo& deviceInfo);

			Device* GetDevice(const uint32_t& id);
			Device* GetDevice(const std::string& name);
			Device* GetDevice(const DeviceInfo& info);

		private:
			const Layout* MatchDeviceLayout(const DeviceInfo& deviceInfo);

		private:
			std::unique_ptr<DeviceBuilder> m_DeviceBuilder;
			std::unordered_map<uint16_t, std::unordered_map<uint32_t, Device>> m_DeviceMap;
			std::unordered_map<uint16_t, std::unordered_map<uint16_t, Layout>> m_LayoutMap;
		};
	}
}