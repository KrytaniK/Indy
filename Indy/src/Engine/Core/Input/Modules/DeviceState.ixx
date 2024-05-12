module;

#include <Engine/Core/LogMacros.h>

#include <vector>
#include <iostream>
#include <typeindex>

export module Indy.Input:DeviceState;

export
{
	namespace Indy
	{
		class DeviceState
		{
		public:
			DeviceState(const size_t& size);

			~DeviceState() = default;

			template<typename T>
			void Write(uint16_t byteOffset, T value);
			void Write(uint16_t byteOffset, std::byte* data, size_t size);
			void WriteBit(uint16_t byteOffset, uint8_t bitOffset, bool value);


			template<typename T>
			T Read(uint16_t byteOffset);
			int ReadBit(uint16_t byteOffset, uint8_t bit);

			size_t Size() { return m_StateBlock.size(); };

		private:
			std::vector<std::byte> m_StateBlock;
		};
		
		// --------------------
		// Template Definitions
		// --------------------

		template<typename T>
		void DeviceState::Write(uint16_t byteOffset, T value)
		{
			if (byteOffset + sizeof(value) > m_StateBlock.size())
			{
				INDY_CORE_ERROR("Attempted to write outside of device state bounds...\nByte Offset: {0}\n Size: {1}", byteOffset, m_StateBlock.size());
				return;
			}

			// Get the raw pointer to the specified region in memory.
			// Do not delete manually.
			std::byte* updateRegion = m_StateBlock.data() + byteOffset;

			// copy the value into that region of memory.
			std::memcpy(updateRegion, &value, sizeof(value));
		}

		template<typename T>
		T DeviceState::Read(uint16_t byteOffset)
		{
			if (byteOffset + sizeof(T) > m_StateBlock.size())
			{
				INDY_CORE_ERROR("Cannot read device state at offset {0} of type [{1}]", byteOffset, typeid(T).name());
				return T();
			}

			T result;

			std::memcpy(&result, &m_StateBlock[byteOffset], sizeof(T));

			return result;
		}
	}
}