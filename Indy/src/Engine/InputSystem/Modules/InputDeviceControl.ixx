module;

#include <string>
#include <functional>
#include <memory>
#include <typeinfo>

export module Indy_Core_InputSystem:DeviceControl;

export
{
	namespace Indy
	{
		// A template structure representing a single partition of backend device state. Used in building virtual devices
		struct InputDeviceControlInfo
		{
			size_t sizeInBits = 0; // [Optional] Size of control in bits
			size_t offset; // Byte offset off this control (negative values represent 'parent' controls, and as such, override parent control data)
			uint8_t bit = 255; // [Optional] Bit offset of this control, relative to byte position
			std::string displayName; // Display name for control
			std::string layout; // Layout for this control (e.g., "Button", "Axis", "Joystick")
			std::string format; // [Optional] Data format for this device (e.g., "VEC2", "VEC3", "VEC4")
			int deviceFormat; // unique ID for owning device

			// Calculates and returns the size of this control in bytes
			size_t Size() const { size_t size = sizeInBits / 8; return size == 0 ? 1 : size; };
		};

		class InputDeviceControlValue
		{
		public:
			InputDeviceControlValue(size_t size, char* controlState, int bit);
			~InputDeviceControlValue();

			size_t GetSize() const { return this->m_Size; };

			template<typename T>
			T ReadAs() const;

		private:
			char* m_RawValue;
			size_t m_Size;
			int m_Bit;
		};

		class InputDeviceControl
		{
		public:
			InputDeviceControl(const InputDeviceControlInfo& info, char* deviceState);
			~InputDeviceControl();

			const InputDeviceControlInfo& GetInfo() { return this->m_Info; };

			void Update();

			void Watch(const std::function<void(const InputDeviceControlValue&)>& onValueChange);

		private:
			InputDeviceControlInfo m_Info;
			std::unique_ptr<InputDeviceControlValue> m_Value;
			std::vector<std::function<void(const InputDeviceControlValue&)>> m_Callbacks;
		};

		// Returns the value of a device control, cast to the desired type.
		// if the value can't be cast due to a size difference, then an empty
		// object of type T is returned.
		template<typename T>
		T InputDeviceControlValue::ReadAs() const
		{
			if (m_Bit >= 0)
			{
				unsigned char bitMask = 1u << m_Bit;
				return static_cast<T>(m_RawValue[0] & bitMask);
			}
			else if (typeid(T) == typeid(int))
				return static_cast<T>(*reinterpret_cast<float*>(m_RawValue));
			else
			{
				size_t copySize = sizeof(T) > m_Size ? m_Size : sizeof(T);

				T value{};
				memcpy(&value, m_RawValue, copySize);
				return value;
			}
		}
	}
};