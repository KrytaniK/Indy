module;

#include <cstdint>
#include <unordered_map>
#include <string>
#include <functional>

#include "Engine/Core/LogMacros.h"

export module Indy.Input:Context;

import :State;
import :Control;
import :Events;

import Indy.Events;

export
{
	namespace Indy::Input
	{
		typedef std::unordered_map<uint32_t, EventHandler> DeviceControlCallbackMap;
		typedef std::unordered_map<uint32_t, DeviceControlCallbackMap> DeviceCallbackMap;

		class StateContext
		{
		public:
			StateContext(Control* control, const std::shared_ptr<InputState>& state);
			~StateContext() = default;

			const uint16_t& GetSize();
			const uint32_t& GetID();
			const std::string& GetName();
			const std::string& GetAlias();

			template<typename T>
			T ReadAs();

		private:
			Control* m_Control;
			std::shared_ptr<InputState> m_State;
		};

		class Context
		{
		public:
			Context();
			~Context() = default;

			std::shared_ptr<EventDelegate> AddInputCallback(const uint32_t& deviceID, const uint32_t& controlID, const std::function<void(CallbackEvent*)>& callback);
			std::shared_ptr<EventDelegate> AddInputCallback(const std::string& deviceName, const std::string& controlName, const std::function<void(CallbackEvent*)>& callback);

			void OnInput(const uint32_t& deviceID, const uint32_t& controlID, StateContext& ctx);

		private:
			CallbackEvent m_CallbackEvent;
			DeviceCallbackMap m_InputCallbacks;
		};

		// Template Definitions

		template <typename T>
		T StateContext::ReadAs()
		{
			const ControlInfo& controlInfo = m_Control->GetInfo();

			if (controlInfo.sizeInBits == 1 || controlInfo.bit != UINT8_MAX)
				return static_cast<T>(m_State->ReadBit(controlInfo.byteOffset, controlInfo.bit));

			return m_State->Read<T>(controlInfo.byteOffset);
		}

	}
}