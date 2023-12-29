#pragma once

#include "InputTypes.h"
#include "InputDevice.h"

#include <vector>
#include <variant>
#include <string>

namespace Engine
{
	class InputManager;

	struct InputElement
	{
		InputDataTypes dataType; // Stored Data Type

		uint16_t offsetIndex; // 0-Based Index offset (This is NOT the memory byte offset)

		size_t GetSize() const
		{
			switch (this->dataType)
			{
				case InputDataTypes::Int: return sizeof(int);
				case InputDataTypes::Float: return sizeof(float);
				case InputDataTypes::Double: return sizeof(double);
				default:
					throw std::runtime_error("Unhandled input type at InputElement::GetSize()");
			}
		}

		template<typename T> 
		T CastToType(char* data) const
		{
			switch (this->dataType)
			{
				case InputDataTypes::Int: return static_cast<T>(*reinterpret_cast<int*>(data));
				case InputDataTypes::Float: return static_cast<T>(*reinterpret_cast<float*>(data));
				case InputDataTypes::Double: return static_cast<T>(*reinterpret_cast<double*>(data));
				default:
					throw std::runtime_error("Unhandled input type at InputElement::CastToType<T>()");
			}
		}
	};
	
	struct InputLayout
	{
		std::string displayname;
		size_t size = 0; // Total size of the stored elements
		std::vector<InputElement> elements;

		void AddLayoutElement(InputElement element)
		{
			switch (element.dataType)
			{
				case InputDataTypes::Int: 
				{
					size += sizeof(int);
					break;
				}
				case InputDataTypes::Float: 
				{
					size += sizeof(float);
					break;
				}
				case InputDataTypes::Double: 
				{
					size += sizeof(double);
					break;
				}
				default:
					throw std::runtime_error("Unhandled input type at InputLayout::AddLayoutElement()");
			}
			element.offsetIndex = (uint16_t)elements.size();
			elements.emplace_back(element);
		}

		const InputElement& GetElementAt(size_t index) const
		{
			if (index > elements.size() || index < 0)
				throw std::runtime_error("Attempted to access out of scope input element!");

			return elements.at(index);
		}

		size_t GetElementOffset(size_t index) const
		{
			size_t offset = 0;

			for (size_t i = 0; i < index; i++)
			{
				offset += this->elements.at(i).GetSize();
			}

			return offset;
		}
	};

	struct InputDescription
	{
		DeviceInput deviceInput;
		void* data; // Raw input data

		uint8_t DeviceID() const { return this->deviceInput.deviceID; };
		uint16_t LayoutID() const { return this->deviceInput.layoutID; };

		template<typename T>
		T GetValueAtIndex(size_t index)
		{
			const InputLayout* layout = InputManager::GetLayout(deviceInput.layoutID);
			const InputElement& element = layout->GetElementAt(index);

			// Ensure we aren't attempting to cast using memory that doesn't belong to the element data.
			if (sizeof(T) > element.GetSize())
				throw std::runtime_error("InputDescription::GetValueAtIndex<T>(size_t index): Attempted to read from out of scope memory.");

			// Convert to a byte array
			char* _data = static_cast<char*>(data);

			// Move the pointer to the requested portion of memory
			_data += layout->GetElementOffset(element.offsetIndex);
			
			// To avoid mismatched value, cast first to the original type, then to type T.
			return element.CastToType<T>(_data);
		}
	};
}