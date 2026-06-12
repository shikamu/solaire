#include "Utilities.h"
#include "InputConstants.h"


Utilities::Utilities()
{
}

Utilities::~Utilities()
{
}

wchar_t* Utilities::getPrintable(const SolaireInputCode code)
{
	switch(code)
	{
		default:
		case INPUT_NONE: 
			return L"Unbound";
		case KEYBOARD_KEY_BACK: return L"BKSP";
		case KEYBOARD_KEY_TAB: return L"TAB";
		case KEYBOARD_KEY_CLEAR: return L"CLEAR";
		case KEYBOARD_KEY_RETURN: return L"ENTER";
		case KEYBOARD_KEY_SHIFT: return L"SHIFT";
		case KEYBOARD_KEY_CONTROL: return L"CTRL";
		case KEYBOARD_KEY_MENU: return L"ALT";
		case KEYBOARD_KEY_PAUSE: return L"PAUSE";
		case KEYBOARD_KEY_CAPITAL: return L"CAPS";
		case KEYBOARD_KEY_ESCAPE: return L"ESC";
		case KEYBOARD_KEY_SPACE: return L"SPACE";
		case KEYBOARD_KEY_PRIOR: return L"PGUP";
		case KEYBOARD_KEY_NEXT: return L"PGDN";
		case KEYBOARD_KEY_END: return L"END";
		case KEYBOARD_KEY_HOME: return L"HOME";
		case KEYBOARD_KEY_LEFT: return L"LEFT";
		case KEYBOARD_KEY_UP: return L"UP";
		case KEYBOARD_KEY_RIGHT: return L"RIGHT";
		case KEYBOARD_KEY_DOWN: return L"DOWN";
		case KEYBOARD_KEY_SNAPSHOT: return L"PRSCR";
		case KEYBOARD_KEY_INSERT: return L"INS";
		case KEYBOARD_KEY_DELETE: return L"DEL";
		case KEYBOARD_KEY_0: return L"0";
		case KEYBOARD_KEY_1: return L"1";
		case KEYBOARD_KEY_2: return L"2";
		case KEYBOARD_KEY_3: return L"3";
		case KEYBOARD_KEY_4: return L"4";
		case KEYBOARD_KEY_5: return L"5";
		case KEYBOARD_KEY_6: return L"6";
		case KEYBOARD_KEY_7: return L"7";
		case KEYBOARD_KEY_8: return L"8";
		case KEYBOARD_KEY_9: return L"9";
		case KEYBOARD_KEY_A: return L"A";
		case KEYBOARD_KEY_B: return L"B";
		case KEYBOARD_KEY_C: return L"C";
		case KEYBOARD_KEY_D: return L"D";
		case KEYBOARD_KEY_E: return L"E";
		case KEYBOARD_KEY_F: return L"F";
		case KEYBOARD_KEY_G: return L"G";
		case KEYBOARD_KEY_H: return L"H";
		case KEYBOARD_KEY_I: return L"I";
		case KEYBOARD_KEY_J: return L"J";
		case KEYBOARD_KEY_K: return L"K";
		case KEYBOARD_KEY_L: return L"L";
		case KEYBOARD_KEY_M: return L"M";
		case KEYBOARD_KEY_N: return L"N";
		case KEYBOARD_KEY_O: return L"O";
		case KEYBOARD_KEY_P: return L"P";
		case KEYBOARD_KEY_Q: return L"Q";
		case KEYBOARD_KEY_R: return L"R";
		case KEYBOARD_KEY_S: return L"S";
		case KEYBOARD_KEY_T: return L"T";
		case KEYBOARD_KEY_U: return L"U";
		case KEYBOARD_KEY_V: return L"V";
		case KEYBOARD_KEY_W: return L"W";
		case KEYBOARD_KEY_X: return L"X";
		case KEYBOARD_KEY_Y: return L"Y";
		case KEYBOARD_KEY_Z: return L"Z";
		case KEYBOARD_KEY_LWIN: return L"LWIN";
		case KEYBOARD_KEY_RWIN: return L"RWIN";
		case KEYBOARD_KEY_APPS: return L"App Key";
		case KEYBOARD_KEY_NUMPAD0: return L"Num0";
		case KEYBOARD_KEY_NUMPAD1: return L"Num1";
		case KEYBOARD_KEY_NUMPAD2: return L"Num2";
		case KEYBOARD_KEY_NUMPAD3: return L"Num3";
		case KEYBOARD_KEY_NUMPAD4: return L"Num4";
		case KEYBOARD_KEY_NUMPAD5: return L"Num5";
		case KEYBOARD_KEY_NUMPAD6: return L"Num6";
		case KEYBOARD_KEY_NUMPAD7: return L"Num7";
		case KEYBOARD_KEY_NUMPAD8: return L"Num8";
		case KEYBOARD_KEY_NUMPAD9: return L"Num9";
		case KEYBOARD_KEY_MULTIPLY: return L"Multiply";
		case KEYBOARD_KEY_ADD: return L"Add";
		case KEYBOARD_KEY_SEPARATOR: return L"Separator";
		case KEYBOARD_KEY_SUBTRACT: return L"Subtract";
		case KEYBOARD_KEY_DECIMAL: return L"Decimal";
		case KEYBOARD_KEY_DIVIDE: return L"Divide";
		case KEYBOARD_KEY_F1: return L"F1";
		case KEYBOARD_KEY_F2: return L"F2";
		case KEYBOARD_KEY_F3: return L"F3";
		case KEYBOARD_KEY_F4: return L"F4";
		case KEYBOARD_KEY_F5: return L"F5";
		case KEYBOARD_KEY_F6: return L"F6";
		case KEYBOARD_KEY_F7: return L"F7";
		case KEYBOARD_KEY_F8: return L"F8";
		case KEYBOARD_KEY_F9: return L"F9";
		case KEYBOARD_KEY_F10: return L"F10";
		case KEYBOARD_KEY_F11: return L"F11";
		case KEYBOARD_KEY_F12: return L"F12";
		case KEYBOARD_KEY_F13: return L"F13";
		case KEYBOARD_KEY_F14: return L"F14";
		case KEYBOARD_KEY_F15: return L"F15";
		case KEYBOARD_KEY_F16: return L"F16";
		case KEYBOARD_KEY_F17: return L"F17";
		case KEYBOARD_KEY_F18: return L"F18";
		case KEYBOARD_KEY_F19: return L"F19";
		case KEYBOARD_KEY_F20: return L"F20";
		case KEYBOARD_KEY_F21: return L"F21";
		case KEYBOARD_KEY_F22: return L"F22";
		case KEYBOARD_KEY_F23: return L"F23";
		case KEYBOARD_KEY_F24: return L"F24";
		case KEYBOARD_KEY_NUMLOCK: return L"NLOCK";
		case KEYBOARD_KEY_SCROLL: return L"SCRLCK";
		case KEYBOARD_KEY_LSHIFT: return L"LSHIFT";
		case KEYBOARD_KEY_RSHIFT: return L"RSHIFT";
		case KEYBOARD_KEY_LCONTROL: return L"LCTRL";
		case KEYBOARD_KEY_RCONTROL: return L"RCTRL";
		case KEYBOARD_KEY_LMENU: return L"LMENU";
		case KEYBOARD_KEY_RMENU: return L"RMENU";
		case KEYBOARD_KEY_OEM_1: return L";";
		case KEYBOARD_KEY_PLUS: return L"PLUS";
		case KEYBOARD_KEY_COMMA: return L".";
		case KEYBOARD_KEY_MINUS: return L"-";
		case KEYBOARD_KEY_PERIOD: return L".";
		case KEYBOARD_KEY_OEM_2: return L"?";
		case KEYBOARD_KEY_OEM_3: return L"~";
		case KEYBOARD_KEY_OEM_4: return L"[";
		case KEYBOARD_KEY_OEM_5: return L"\\";
		case KEYBOARD_KEY_OEM_6: return L"]";
		case KEYBOARD_KEY_OEM_7: return L"'";
		case KEYBOARD_KEY_OEM_102 : return L"<>";

		case MOUSE_WHEEL_UP: return L"Wheel UP";
		case MOUSE_WHEEL_DOWN: return L"Wheel DN";
		case MOUSE_LEFT_CLICK: return L"Left Click";
		case MOUSE_RIGHT_CLICK: return L"Right Click";
		case MOUSE_MIDDLE_CLICK: return L"Middle Click";
		case MOUSE_MOVE_LEFT: return L"Move Mouse Left";
		case MOUSE_MOVE_RIGHT: return L"Move Mouse Right";
		case MOUSE_MOVE_UP: return L"Move Mouse Up";
		case MOUSE_MOVE_DOWN: return L"Move Mouse Down";

		case JOYSTICK_AXIS_X: return L"Axis X";
		case JOYSTICK_AXIS_Y: return L"Axis Y";
		case JOYSTICK_AXIS_Z: return L"Axis Z";
		case JOYSTICK_AXIS_R: return L"Axis R";
		case JOYSTICK_AXIS_U: return L"Axis U";
		case JOYSTICK_AXIS_V: return L"Axis V";

		case JOYSTICK_BUTTON_0: return L"Button 0";
		case JOYSTICK_BUTTON_1: return L"Button 1";
		case JOYSTICK_BUTTON_2: return L"Button 2";
		case JOYSTICK_BUTTON_3: return L"Button 3";
		case JOYSTICK_BUTTON_4: return L"Button 4";
		case JOYSTICK_BUTTON_5: return L"Button 5";
		case JOYSTICK_BUTTON_6: return L"Button 6";
		case JOYSTICK_BUTTON_7: return L"Button 7";
		case JOYSTICK_BUTTON_8: return L"Button 8";
		case JOYSTICK_BUTTON_9: return L"Button 9";
		case JOYSTICK_BUTTON_10: return L"Button 10";
		case JOYSTICK_BUTTON_11: return L"Button 11";
		case JOYSTICK_BUTTON_12: return L"Button 12";
		case JOYSTICK_BUTTON_13: return L"Button 13";
		case JOYSTICK_BUTTON_14: return L"Button 14";
		case JOYSTICK_BUTTON_15: return L"Button 15";
		case JOYSTICK_BUTTON_16: return L"Button 16";
		case JOYSTICK_BUTTON_17: return L"Button 17";
		case JOYSTICK_BUTTON_18: return L"Button 18";
		case JOYSTICK_BUTTON_19: return L"Button 19";
		case JOYSTICK_BUTTON_20: return L"Button 20";
		case JOYSTICK_BUTTON_21: return L"Button 21";
		case JOYSTICK_BUTTON_22: return L"Button 22";
		case JOYSTICK_BUTTON_23: return L"Button 23";
		case JOYSTICK_BUTTON_24: return L"Button 24";
		case JOYSTICK_BUTTON_25: return L"Button 25";
		case JOYSTICK_BUTTON_26: return L"Button 26";
		case JOYSTICK_BUTTON_27: return L"Button 27";
		case JOYSTICK_BUTTON_28: return L"Button 28";
		case JOYSTICK_BUTTON_29: return L"Button 29";
		case JOYSTICK_BUTTON_30: return L"Button 30";
		case JOYSTICK_BUTTON_31: return L"Button 31";
	}
}

wchar_t* Utilities::getActionName(const Action action)
{
	switch(action)
	{
		default:
			return L"";
		case GAME_MENU: return L"Game Menu";
		case PITCH_MORE: return L"Pitch More"; 
		case PITCH_LESS: return L"Pitch Less";
		case YAW_MORE: return L"Yaw More";
		case YAW_LESS: return L"Yaw Less";
		case ROLL_MORE: return L"Roll More";
		case ROLL_LESS: return L"Roll Less";
		case THRUST_MORE: return L"More Thrust";
		case THRUST_LESS: return L"Less Thrust";
		case ZOOM_IN: return L"Zoom In";
		case ZOOM_OUT: return L"Zoom Out";
		case ACTION_1: return L"Primary";
		case ACTION_2: return L"Secondary";
		case ACTION_3: return L"Tertiary";
		case ACTION_4: return L"Next Target";
		case ACTION_5: return L"Previous Target";
		case ACTION_6: return L"Front Target";
		case ACTION_7: return L"Show Scores";
		case ACTION_8: return L"Pause";
		case ACTION_9: return L"Action 9";
		case ACTION_10: return L"Action 10";
	}
}

