#ifndef GAME_ENGINE_INPUT_H
#define GAME_ENGINE_INPUT_H
#include "../math/math_lib.h"

class window;

class input
{
public:
	enum
	{
		KEY_A = 4,

		KEY_D = 7,
		KEY_E = 8,

		KEY_K = 14,
		KEY_L = 15,

		KEY_Q = 20,
		KEY_S = 22,
		KEY_V = 25,
		KEY_W = 26,

		KEY_1 = 30,
		KEY_2 = 31,
		KEY_4 = 33,
		KEY_5 = 34,

        KEY_ESCAPE = 41,
        KEY_TAB = 43,
        KEY_SPACE = 44,

        MOUSE_LEFT_BUTTON = 1,
        MOUSE_MIDDLE_BUTTON = 2,
        MOUSE_RIGHT_BUTTON = 3,
	};

	static const int KEYS = 45;
	static const int MOUSE_BUTTONS = 6;

	input(window* window);

	inline bool get_key_down(int key_code) const { return key_down[key_code]; }
	inline bool get_key_up(int key_code) const { return key_up[key_code]; }
	inline bool get_mouse_down(int key_code) const { return mouse_down[key_code]; }
	inline bool get_mouse_up(int key_code) const { return mouse_up[key_code]; }
	inline vec2f get_mouse_pos() const { return vec2f((float) mouse_y, (float) mouse_x); }
	
	void set_cursor(bool show) const;
    void set_mouse_pos(const vec2f &pos) const;
	
	inline void set_key_down(int key_code, bool state) { key_down[key_code] = state; }
	inline void set_key_up(int key_code, bool state) { key_up[key_code] = state; }
	inline void set_mouse_down(int key_code, bool state) { mouse_down[key_code] = state; }
	inline void set_mouse_up(int key_code, bool state) { mouse_up[key_code] = state; }
	inline void set_mouse_y(int y) { mouse_y = y; }
	inline void set_mouse_x(int x) { mouse_x = x; }

private:
	bool key_down[KEYS];
	bool key_up[KEYS];
	bool mouse_down[MOUSE_BUTTONS];
	bool mouse_up[MOUSE_BUTTONS];
	int mouse_y, mouse_x;
	window* m_window;
};

#endif