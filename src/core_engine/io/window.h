#ifndef GAME_ENGINE_WINDOW_H
#define GAME_ENGINE_WINDOW_H

#include <SDL2/SDL.h>
#include <string>
#include "input.h"

class window
{
public:
	window(int width, int height, const std::string& title);
	~window();
	
	void update();
	void swap_buffers();
	static void bind_as_render_targets();

	inline bool is_closed_requested()          const { return close_requested; }
	inline int get_width()                   const { return width; }
	inline int get_height()                  const { return height; }
	inline float get_aspect()                const { return (float)WIN_WIDTH/(float)WIN_HEIGHT; }
	inline const std::string& get_title()    const { return title; }
	inline vec2f get_center()             const { return vec2f((float)width/2.0f, (float)height/2.0f); }
	inline SDL_Window* get_sdl_window()             { return sdl_window; }
	inline const input& get_input()          const { return win_input; }
    
    window(const window& other) : win_input(this) {}
    void operator=(const window& other) {}

	void set_full_screen(bool value);
	static const int WIN_WIDTH = 824;
	static const int WIN_HEIGHT = 600;
private:
	int width;
	int height;
	int old_width;
	int old_height;
	std::string title;
	SDL_Window* sdl_window;
	SDL_GLContext gl_context;
	input win_input;
	bool close_requested;
};

#endif