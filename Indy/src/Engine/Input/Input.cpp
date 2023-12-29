#include "Input.h"

#include "Engine/Core/Log.h"
#include "InputDescription.h"

/* Next major step for this system:
*	- Allow for the registering of input devices, layouts and outputs to
*		come from an external file (probably JSON), to make using this
*		system easier.
* 
*	- Currently, the system only works exclusively through the Event system.
*		It would be a nice feature for this behaviour to be optional. Something
*		along the lines of a ProcessInput function that takes in an input
*		description, providing the same functionality.
* 
*  TODO:
*	- InputOutputs (Think of a better name). These map to input layouts,
		and are updated every time that layout is used for input through
		an onValueChange function that uses an input description to map
		the input data to the correct types. Think things such as mouse
		position, direction, delta, etc. Each use the same input layout,
		but handle input differently.

	- InputBindings: An intermediary structure for binding outputs to
		various "input events". Events in this sense are different from the
		events in the Event system in that they are bound to the domain
		of the Input system, and behave strictly based on bound input.
		Input Bindings serve as a way to describe the binding of an input's 
		output to specific input events.

	- InputComposites: Another intermediary structure similar in nature to
		Input Bindings. The difference here, is that an input composite
		can contain multiple input bindings. There are restrictions to the
		number of bindings an input composite can hold, which are defined
		per composite.

	- InputEvents: (Think of a better name) These serve as "effects" of
		any input that occurs in the application. These are can be bound 
		to multiple of either input composites, or input bindings. Input Events
		operate only in the domain of Input, and don't define their own behavior.
		Instead, the behavior of these events is user-defined and ready for any
		use case.
*/

namespace Engine
{
	std::unordered_map<std::string, InputDevice> InputManager::s_Devices;
	std::vector<InputLayout> InputManager::s_Layouts;

	void InputManager::Init()
	{
		Events::Bind("Input", "Raw", InputManager::OnInput);

		InputDevice mouse;
		mouse.displayName = "Mouse";
		mouse.id = 0;

		// Default Mouse Button Input Layout
		InputLayout mouseButtonLayout;
		mouseButtonLayout.displayname = "Button";
		mouseButtonLayout.AddLayoutElement({ InputDataTypes::Int }); // Integer Button ID
		mouseButtonLayout.AddLayoutElement({ InputDataTypes::Int }); // Integer Action ID
		mouseButtonLayout.AddLayoutElement({ InputDataTypes::Int }); // Integer Modification ID

		// Default Mouse Scroll Input Layout
		InputLayout mouseScrollLayout;
		mouseScrollLayout.displayname = "Scroll";
		mouseScrollLayout.AddLayoutElement({ InputDataTypes::Double }); // X Scroll Offset
		mouseScrollLayout.AddLayoutElement({ InputDataTypes::Double }); // Y Scroll Offset

		// Default Mouse Position Layout
		InputLayout mousePositionLayout;
		mousePositionLayout.displayname = "Position";
		mousePositionLayout.AddLayoutElement({ InputDataTypes::Double }); // X Position
		mousePositionLayout.AddLayoutElement({ InputDataTypes::Double }); // Y Position

		// Register Default Mouse
		InputManager::RegisterDevice(mouse);
		InputManager::RegisterDeviceLayout(mouse, mouseButtonLayout);
		InputManager::RegisterDeviceLayout(mouse, mouseScrollLayout);
		InputManager::RegisterDeviceLayout(mouse, mousePositionLayout);

		InputDevice keyboard;
		keyboard.displayName = "Keyboard";
		keyboard.id = 1;

		// Default Keyboard Key Layout
		InputLayout keyboardKeyLayout;
		keyboardKeyLayout.displayname = "Key";
		keyboardKeyLayout.AddLayoutElement({ InputDataTypes::Int }); // Integer Key ID
		keyboardKeyLayout.AddLayoutElement({ InputDataTypes::Int }); // Integer Scancode ID
		keyboardKeyLayout.AddLayoutElement({ InputDataTypes::Int }); // Integer Action ID
		keyboardKeyLayout.AddLayoutElement({ InputDataTypes::Int }); // Integer Modification ID

		// Register Default Keyboard
		InputManager::RegisterDevice(keyboard);
		InputManager::RegisterDeviceLayout(keyboard, keyboardKeyLayout);
	}

	void InputManager::Shutdown()
	{
		
	}

	void InputManager::OnInput(Event& event)
	{
		InputDescription* inputDesc = (InputDescription*)event.data;

		
	}

	void InputManager::RegisterDevice(InputDevice device)
	{
		auto deviceIt = s_Devices.find(device.displayName);
		if (deviceIt != s_Devices.end())
			throw std::runtime_error("Failed to register input device. Reason: Device already exists!");

		s_Devices.insert(std::make_pair(device.displayName, device));
	}

	InputDevice* InputManager::GetDevice(std::string displayName)
	{
		auto deviceIt = s_Devices.find(displayName);
		if (deviceIt == s_Devices.end())
			throw std::runtime_error("Failed to get input device. Reason: Device does not exist! To register a device, use InputManager::RegisterDevice();");

		return &deviceIt->second;
	}

	uint8_t InputManager::GetDeviceID(std::string displayName)
	{
		InputDevice* device = InputManager::GetDevice(displayName);
		return device->id;
	}

	uint16_t InputManager::RegisterLayout(InputLayout layout) {
		s_Layouts.emplace_back(layout);
		return static_cast<uint16_t>(s_Layouts.size() - 1);
	};

	uint16_t InputManager::RegisterDeviceLayout(InputDevice device, InputLayout layout)
	{
		// Retrieve the registered device. Guards agains unregistered devices.
		InputDevice* reg_device = InputManager::GetDevice(device.displayName);

		uint16_t layoutID = RegisterLayout(layout);
		reg_device->layouts.insert(std::make_pair(layout.displayname, layoutID));

		return layoutID;
	}

	uint16_t InputManager::RegisterDeviceLayout(std::string deviceName, InputLayout layout)
	{
		InputDevice* device = InputManager::GetDevice(deviceName);

		uint16_t layoutID = RegisterLayout(layout);
		device->layouts.insert(std::make_pair(layout.displayname, layoutID));

		return layoutID;
	}

	InputLayout* InputManager::GetLayout(uint16_t index) 
	{ 
		if (index > s_Layouts.size() || index < 0)
			return nullptr;

		return &s_Layouts[index]; 
	};

	uint16_t InputManager::GetDeviceLayoutID(std::string deviceName, std::string layoutName)
	{
		InputDevice* device = InputManager::GetDevice(deviceName);

		auto layoutIDIt = device->layouts.find(layoutName);
		if (layoutIDIt == device->layouts.end())
			throw std::runtime_error("Failed to get device layout ID. Reason: Layout does not exist!");

		return layoutIDIt->second;
	}

	InputLayout* InputManager::GetDeviceLayout(std::string deviceName, std::string layoutName)
	{
		InputDevice* device = InputManager::GetDevice(deviceName);

		auto layoutIDIt = device->layouts.find(layoutName);
		if (layoutIDIt == device->layouts.end())
			return nullptr;

		return &s_Layouts[layoutIDIt->second];
	}
}