#include "Engine/Core/LogMacros.h"

#include <iostream>

import Indy_Core_Input;

namespace Indy
{
	DeviceState::DeviceState(const size_t& size)
	{
		m_StateBlock.resize(size);
	}

	void DeviceState::Write(uint16_t byteOffset, std::byte* data, size_t size)
	{
		if (byteOffset + size > m_StateBlock.size())
		{
			INDY_CORE_ERROR("Attempted to write outside of device state bounds...\nByte Offset: {0}\n State Size: {1}\nSize: {2}\nOffset & Size: {3}", byteOffset, m_StateBlock.size(), size, byteOffset + size);
			return;
		}

		std::byte* state = m_StateBlock.data() + byteOffset;

		memcpy(state, data, size);
	}

	void DeviceState::WriteBit(uint16_t byteOffset, uint8_t bitOffset, bool value)
	{
		if (byteOffset > m_StateBlock.size())
		{
			INDY_CORE_ERROR("Attempted to write outside of device state bounds...\nByte Offset: {0}\n Size: {1}", byteOffset, m_StateBlock.size());
			return;
		}

		if (value)
			m_StateBlock[byteOffset] |= std::byte{1} << bitOffset; // Set bit to 1
		else
			m_StateBlock[byteOffset] &= ~(std::byte{1} << bitOffset); // Set bit to 0
	}

	int DeviceState::ReadBit(uint16_t byteOffset, uint8_t bit)
	{
		if (byteOffset > m_StateBlock.size())
		{
			INDY_CORE_ERROR("Attempted to write outside of device state bounds...");
			return false;
		}

		// Create a mask to isolate target bit with. (Ensure the bit is less than 8)
		std::byte bitMask = std::byte{1} << (bit % 8);

		// Logical AND the byte value with the bitmask to isolate the target bit.
		std::byte result = m_StateBlock[byteOffset] & bitMask;

		// shift the bit to the front of the byte for reading
		return static_cast<bool>(result);
	}
}