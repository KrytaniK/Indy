#include "Engine/Core/LogMacros.h"

// Remove later
#include <GLFW/glfw3.h>
#include <string>

#include <memory>

import Indy.Application;
import Indy.Input;
import Indy.Events;
import Indy.Profiler;

namespace Indy
{
	using namespace Input;

	InputSystem::InputSystem()
	{
		m_DeviceManager = std::make_unique<DeviceManager>();

		// Sync Application Events
		Application& app = Application::Get();
		app.OnLoad.Subscribe<InputSystem>(this, &InputSystem::OnLoad);
		app.OnUnload.Subscribe<InputSystem>(this, &InputSystem::OnUnload);

		// Bind Event Handles
		Events<Input::Event>::Subscribe<InputSystem>(this, &InputSystem::OnInput);
		Events<Input::DeviceGetEvent>::Subscribe<InputSystem>(this, &InputSystem::OnGetDevice);
		Events<Input::SetContextEvent>::Subscribe<InputSystem>(this, &InputSystem::OnSetContext);
	}

	void InputSystem::OnLoad()
	{
		// Remove later
		DeviceInfo glfwMouseInfo;
		glfwMouseInfo.displayName = "GLFW Mouse";
		glfwMouseInfo.deviceClass = 0x0000;
		glfwMouseInfo.layoutID = 0x4D53;
		glfwMouseInfo.id = 0;

		Layout glfwMouseLayout;
		glfwMouseLayout.displayName = "GLFW Mouse";
		glfwMouseLayout.deviceClass = 0x0000;
		glfwMouseLayout.id = 0x4D53;
		glfwMouseLayout.sizeInBytes = (sizeof(double) * 4) + 1;
		glfwMouseLayout.controls = {
			{std::to_string(GLFW_MOUSE_BUTTON_1), "Left Mouse Button", 0, 0, 1, 0, 0, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_2), "Right Mouse Button", 1, 0, 1, 0, 1, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_3), "Middle Mouse Button", 2, 0, 1, 0, 2, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_4), "", 3, 0, 1, 0, 3, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_5), "", 4, 0, 1, 0, 4, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_6), "", 5, 0, 1, 0, 5, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_7), "", 6, 0, 1, 0, 6, 0},
			{std::to_string(GLFW_MOUSE_BUTTON_8), "", 7, 0, 1, 0, 7, 0},
			{"Position", "", 8,		sizeof(double) * 2,		sizeof(double) * 2 * 8,		1, 0xFF, 2},
			{"X",		"", 9,		sizeof(double),			sizeof(double) * 8,			1, 0,		0},
			{"Y",		"", 10,		sizeof(double),			sizeof(double) * 8,			5, 0,		0},
			{"Scroll",	"", 11,		sizeof(double) * 2,		sizeof(double) * 2 * 8,		1, 0xFF, 2},
			{"X",		"", 12,		sizeof(double),			sizeof(double) * 8,			1, 0,		0},
			{"Y",		"", 13,		sizeof(double),			sizeof(double) * 8,			5, 0,		0},
		};

		m_DeviceManager->AddLayout(glfwMouseLayout);
		m_DeviceManager->AddDevice(glfwMouseInfo);
	}

	void InputSystem::OnUnload()
	{
		
	}

	void InputSystem::OnInput(Input::Event* event)
	{
		Device* device = nullptr;

		if (event->device_id.has_value())
			device = m_DeviceManager->GetDevice(event->device_id.value());
		else
			device = m_DeviceManager->GetDevice(event->device_name);

		if (!device)
		{
			INDY_CORE_ERROR("Could not process input: Bad device.");
			return;
		}

		Control* control = nullptr;

		if (event->control_id.has_value())
			control = device->GetControl(event->control_id.value());
		else
			control = device->GetControl(event->control_alias);

		if (!control)
		{
			INDY_CORE_ERROR("Could not process input: Bad control.");
			return;
		}

		control->Update(static_cast<std::byte*>(event->data));

		if (m_ActiveContext)
		{
			StateContext ctx(control, device->GetState());
			m_ActiveContext->OnInput(device->GetID(), control->GetID(), ctx);
		}
	}

	void InputSystem::OnGetDevice(Input::DeviceGetEvent* event)
	{
		if (event->device_id.has_value())
			event->outDevice = m_DeviceManager->GetDevice(event->device_id.value());
		else
			event->outDevice = m_DeviceManager->GetDevice(event->device_name);

		if (!event->outDevice)
			INDY_CORE_CRITICAL("Could not get device");
	}

	void InputSystem::OnSetContext(Input::SetContextEvent* event)
	{
		if (!event->newContext)
			return;

		m_ActiveContext = event->newContext;
	}
}
