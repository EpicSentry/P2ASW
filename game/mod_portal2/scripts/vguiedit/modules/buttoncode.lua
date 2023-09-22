
local ButtonCode = {}


-- enum helpers.

ButtonCode.MAX_SPLITSCREEN_CLIENT_BITS = 2

-- This is ( 1 << MAX_SPLITSCREEN_CLIENT_BITS )
ButtonCode.MAX_SPLITSCREEN_CLIENTS = 4 

ButtonCode.MAX_JOYSTICKS = ButtonCode.MAX_SPLITSCREEN_CLIENTS
ButtonCode.MOUSE_BUTTON_COUNT = 5
	
ButtonCode.JOY_AXIS_X = 0
ButtonCode.JOY_AXIS_Y = 1
ButtonCode.JOY_AXIS_Z = 2
ButtonCode.JOY_AXIS_R = 3
ButtonCode.JOY_AXIS_U = 4
ButtonCode.JOY_AXIS_V = 5
ButtonCode.MAX_JOYSTICK_AXES = 6

ButtonCode.JOYSTICK_MAX_BUTTON_COUNT = 32
ButtonCode.JOYSTICK_POV_BUTTON_COUNT = 4
ButtonCode.JOYSTICK_AXIS_BUTTON_COUNT = ButtonCode.MAX_JOYSTICK_AXES * 2





ButtonCode.BUTTON_CODE_INVALID = -1
ButtonCode.BUTTON_CODE_NONE = 0

ButtonCode.KEY_FIRST = 0
ButtonCode.KEY_NONE = ButtonCode.KEY_FIRST
ButtonCode.KEY_0 = 1
ButtonCode.KEY_1 = 2
ButtonCode.KEY_2 = 3
ButtonCode.KEY_3 = 4
ButtonCode.KEY_4 = 5
ButtonCode.KEY_5 = 6
ButtonCode.KEY_6 = 7
ButtonCode.KEY_7 = 8
ButtonCode.KEY_8 = 9
ButtonCode.KEY_9 = 10
ButtonCode.KEY_A = 11
ButtonCode.KEY_B = 12
ButtonCode.KEY_C = 13
ButtonCode.KEY_D = 14
ButtonCode.KEY_E = 15
ButtonCode.KEY_F = 16
ButtonCode.KEY_G = 17
ButtonCode.KEY_H = 18
ButtonCode.KEY_I = 19
ButtonCode.KEY_J = 20
ButtonCode.KEY_K = 21
ButtonCode.KEY_L = 22
ButtonCode.KEY_M = 23
ButtonCode.KEY_N = 24
ButtonCode.KEY_O = 25
ButtonCode.KEY_P = 26
ButtonCode.KEY_Q = 27
ButtonCode.KEY_R = 28
ButtonCode.KEY_S = 29
ButtonCode.KEY_T = 30
ButtonCode.KEY_U = 31
ButtonCode.KEY_V = 32
ButtonCode.KEY_W = 33
ButtonCode.KEY_X = 34
ButtonCode.KEY_Y = 35
ButtonCode.KEY_Z = 36
ButtonCode.KEY_PAD_0 = 37
ButtonCode.KEY_PAD_1 = 38
ButtonCode.KEY_PAD_2 = 39
ButtonCode.KEY_PAD_3 = 40
ButtonCode.KEY_PAD_4 = 41
ButtonCode.KEY_PAD_5 = 42
ButtonCode.KEY_PAD_6 = 43
ButtonCode.KEY_PAD_7 = 44
ButtonCode.KEY_PAD_8 = 45
ButtonCode.KEY_PAD_9 = 46
ButtonCode.KEY_PAD_DIVIDE = 47
ButtonCode.KEY_PAD_MULTIPLY = 48
ButtonCode.KEY_PAD_MINUS = 49
ButtonCode.KEY_PAD_PLUS = 50
ButtonCode.KEY_PAD_ENTER = 51
ButtonCode.KEY_PAD_DECIMAL = 52
ButtonCode.KEY_LBRACKET = 53
ButtonCode.KEY_RBRACKET = 54
ButtonCode.KEY_SEMICOLON = 55
ButtonCode.KEY_APOSTROPHE = 56
ButtonCode.KEY_BACKQUOTE = 57
ButtonCode.KEY_COMMA = 58
ButtonCode.KEY_PERIOD = 59
ButtonCode.KEY_SLASH = 60
ButtonCode.KEY_BACKSLASH = 61
ButtonCode.KEY_MINUS = 62
ButtonCode.KEY_EQUAL = 63
ButtonCode.KEY_ENTER = 64
ButtonCode.KEY_SPACE = 65
ButtonCode.KEY_BACKSPACE = 66
ButtonCode.KEY_TAB = 67
ButtonCode.KEY_CAPSLOCK = 68
ButtonCode.KEY_NUMLOCK = 69
ButtonCode.KEY_ESCAPE = 70
ButtonCode.KEY_SCROLLLOCK = 71
ButtonCode.KEY_INSERT = 72
ButtonCode.KEY_DELETE = 73
ButtonCode.KEY_HOME = 74
ButtonCode.KEY_END = 75
ButtonCode.KEY_PAGEUP = 76
ButtonCode.KEY_PAGEDOWN = 77
ButtonCode.KEY_BREAK = 78
ButtonCode.KEY_LSHIFT = 79
ButtonCode.KEY_RSHIFT = 80
ButtonCode.KEY_LALT = 81
ButtonCode.KEY_RALT = 82
ButtonCode.KEY_LCONTROL = 83
ButtonCode.KEY_RCONTROL = 84
ButtonCode.KEY_LWIN = 85
ButtonCode.KEY_RWIN = 86
ButtonCode.KEY_APP = 87
ButtonCode.KEY_UP = 88
ButtonCode.KEY_LEFT = 89
ButtonCode.KEY_DOWN = 90
ButtonCode.KEY_RIGHT = 91
ButtonCode.KEY_F1 = 92
ButtonCode.KEY_F2 = 93
ButtonCode.KEY_F3 = 94
ButtonCode.KEY_F4 = 95
ButtonCode.KEY_F5 = 96
ButtonCode.KEY_F6 = 97
ButtonCode.KEY_F7 = 98
ButtonCode.KEY_F8 = 99
ButtonCode.KEY_F9 = 100
ButtonCode.KEY_F10 = 101
ButtonCode.KEY_F11 = 102
ButtonCode.KEY_F12 = 103
ButtonCode.KEY_CAPSLOCKTOGGLE = 104
ButtonCode.KEY_NUMLOCKTOGGLE = 105
ButtonCode.KEY_SCROLLLOCKTOGGLE = 106

-- 106
ButtonCode.KEY_LAST = ButtonCode.KEY_SCROLLLOCKTOGGLE
ButtonCode.KEY_COUNT = ButtonCode.KEY_LAST - ButtonCode.KEY_FIRST + 1
	
-- 107
ButtonCode.MOUSE_FIRST = ButtonCode.KEY_LAST + 1
ButtonCode.MOUSE_LEFT = ButtonCode.MOUSE_FIRST
ButtonCode.MOUSE_RIGHT = 108
ButtonCode.MOUSE_MIDDLE = 109
ButtonCode.MOUSE_4 = 110
ButtonCode.MOUSE_5 = 111
ButtonCode.MOUSE_WHEEL_UP = 112
ButtonCode.MOUSE_WHEEL_DOWN = 113

ButtonCode.MOUSE_LAST = ButtonCode.MOUSE_WHEEL_DOWN
ButtonCode.MOUSE_COUNT = ButtonCode.MOUSE_LAST - ButtonCode.MOUSE_FIRST + 1

ButtonCode.JOYSTICK_FIRST = ButtonCode.MOUSE_LAST + 1
ButtonCode.JOYSTICK_FIRST_BUTTON = ButtonCode.JOYSTICK_FIRST
ButtonCode.JOYSTICK_LAST_BUTTON = ButtonCode.JOYSTICK_FIRST_BUTTON + ((ButtonCode.MAX_JOYSTICKS-1) * ButtonCode.JOYSTICK_MAX_BUTTON_COUNT) + (ButtonCode.JOYSTICK_MAX_BUTTON_COUNT-1) 
ButtonCode.JOYSTICK_FIRST_POV_BUTTON = ButtonCode.JOYSTICK_LAST_BUTTON + 1
ButtonCode.JOYSTICK_LAST_POV_BUTTON = ButtonCode.JOYSTICK_FIRST_POV_BUTTON + ((ButtonCode.MAX_JOYSTICKS-1) * ButtonCode.JOYSTICK_POV_BUTTON_COUNT) + (ButtonCode.JOYSTICK_POV_BUTTON_COUNT-1) 
ButtonCode.JOYSTICK_FIRST_AXIS_BUTTON = ButtonCode.JOYSTICK_LAST_POV_BUTTON + 1
ButtonCode.JOYSTICK_LAST_AXIS_BUTTON = ButtonCode.JOYSTICK_FIRST_AXIS_BUTTON + ((ButtonCode.MAX_JOYSTICKS-1) * ButtonCode.JOYSTICK_AXIS_BUTTON_COUNT) + (ButtonCode.JOYSTICK_AXIS_BUTTON_COUNT-1) 
ButtonCode.JOYSTICK_LAST = ButtonCode.JOYSTICK_LAST_AXIS_BUTTON

ButtonCode.BUTTON_CODE_LAST = ButtonCode.JOYSTICK_LAST + 1
ButtonCode.BUTTON_CODE_COUNT = ButtonCode.BUTTON_CODE_LAST - ButtonCode.KEY_FIRST + 1

-- Helpers for XBox 360
ButtonCode.KEY_XBUTTON_UP = ButtonCode.JOYSTICK_FIRST_POV_BUTTON	
ButtonCode.KEY_XBUTTON_RIGHT = ButtonCode.KEY_XBUTTON_UP + 1
ButtonCode.KEY_XBUTTON_DOWN = ButtonCode.KEY_XBUTTON_RIGHT + 1
ButtonCode.KEY_XBUTTON_LEFT = ButtonCode.KEY_XBUTTON_DOWN + 1

ButtonCode.KEY_XBUTTON_A = ButtonCode.JOYSTICK_FIRST_BUTTON		
ButtonCode.KEY_XBUTTON_B = ButtonCode.KEY_XBUTTON_A + 1
ButtonCode.KEY_XBUTTON_X = ButtonCode.KEY_XBUTTON_B + 1
ButtonCode.KEY_XBUTTON_Y = ButtonCode.KEY_XBUTTON_X + 1
ButtonCode.KEY_XBUTTON_LEFT_SHOULDER = ButtonCode.KEY_XBUTTON_Y + 1
ButtonCode.KEY_XBUTTON_RIGHT_SHOULDER = ButtonCode.KEY_XBUTTON_LEFT_SHOULDER + 1
ButtonCode.KEY_XBUTTON_BACK = ButtonCode.KEY_XBUTTON_RIGHT_SHOULDER + 1
ButtonCode.KEY_XBUTTON_START = ButtonCode.KEY_XBUTTON_BACK + 1
ButtonCode.KEY_XBUTTON_STICK1 = ButtonCode.KEY_XBUTTON_START + 1
ButtonCode.KEY_XBUTTON_STICK2 = ButtonCode.KEY_XBUTTON_STICK1 + 1
ButtonCode.KEY_XBUTTON_INACTIVE_START = ButtonCode.KEY_XBUTTON_STICK2 + 1

ButtonCode.KEY_XSTICK1_RIGHT = ButtonCode.JOYSTICK_FIRST_AXIS_BUTTON	
ButtonCode.KEY_XSTICK1_LEFT = ButtonCode.KEY_XSTICK1_RIGHT + 1							
ButtonCode.KEY_XSTICK1_DOWN = ButtonCode.KEY_XSTICK1_LEFT + 1							
ButtonCode.KEY_XSTICK1_UP = ButtonCode.KEY_XSTICK1_DOWN + 1
ButtonCode.KEY_XBUTTON_LTRIGGER = ButtonCode.KEY_XSTICK1_UP + 1						
ButtonCode.KEY_XBUTTON_RTRIGGER = ButtonCode.KEY_XBUTTON_LTRIGGER + 1						
ButtonCode.KEY_XSTICK2_RIGHT = ButtonCode.KEY_XBUTTON_RTRIGGER + 1							
ButtonCode.KEY_XSTICK2_LEFT = ButtonCode.KEY_XSTICK2_RIGHT + 1							
ButtonCode.KEY_XSTICK2_DOWN = ButtonCode.KEY_XSTICK2_LEFT + 1							
ButtonCode.KEY_XSTICK2_UP = ButtonCode.KEY_XSTICK2_DOWN + 1								
	

return ButtonCode	


	



