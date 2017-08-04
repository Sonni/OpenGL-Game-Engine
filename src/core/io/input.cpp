#include "input.h"
#include "window.h"

input::input(window* window) :
	mouse_y(0),
	mouse_x(0),
	m_window(window)
{
	memset(key_down, 0, KEYS * sizeof(bool));
	memset(key_up, 0, KEYS * sizeof(bool));
	
	memset(mouse_down, 0, MOUSE_BUTTONS * sizeof(bool));
	memset(mouse_up, 0, MOUSE_BUTTONS * sizeof(bool));
}

void input::set_cursor(bool show) const
{
	if(show)
    {
        SDL_ShowCursor(1);
    }
	else
    {
        SDL_ShowCursor(0);
    }
}

void input::set_mouse_pos(const vec2f &pos) const
{
	SDL_WarpMouseInWindow(m_window->get_sdl_window(), (int) pos.get_x(), (int) pos.get_y());
}
