#ifndef GAME_ENGINE_WINDOW_H
#define GAME_ENGINE_WINDOW_H

#include <SDL2/SDL.h>
#include "input.h"

class window
{
public:
	window(int width, int height, const std::string& title);
	~window();
	
	void update();
	void swap_buffers();
	static void bind_as_render_targets();

	inline bool is_closed_requested()          const { return m_close_requested; }
	inline int get_width()                   const { return m_width; }
	inline int get_height()                  const { return m_height; }
	inline float get_aspect()                const { return (float)WIN_WIDTH / (float)WIN_HEIGHT; }
	inline const std::string& get_title()    const { return m_title; }
	inline vec2f get_center()             const { return vec2f((float)m_width/2.0f, (float)m_height/2.0f); }
	inline SDL_Window* get_sdl_window()             { return m_sdl_window; }
	inline const input& get_input()          const { return m_win_input; }
    
    window(const window& other) : m_win_input(this) {}
    void operator=(const window& other) {}

	void set_full_screen(bool value);
	static const int WIN_WIDTH = 824;
	static const int WIN_HEIGHT = 600;
private:
	int m_width;
	int m_height;
	int m_old_width;
	int m_old_height;
	std::string m_title;
	SDL_Window* m_sdl_window;
	SDL_GLContext m_gl_context;
	input m_win_input;
	bool m_close_requested;
};

#endif