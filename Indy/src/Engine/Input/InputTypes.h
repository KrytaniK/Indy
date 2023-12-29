#pragma once

namespace Engine
{
	enum InputDataTypes { Int = 0, Float, Double };
	enum InputTypes { Button = 0, Scroll, Axis, Axis2D, Axis3D, Axis4D };
	enum InputActions { Press = 0, Release, Hold, Unknown };
	enum InputModifiers { LControl = 0, RControl, LAlt, RAlt, LShift, RShift, LSuper, RSuper, CapsLck, NumLck };
}