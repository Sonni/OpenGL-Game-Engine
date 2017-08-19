#include "window.h"
#include <GL/glew.h>

window::window(int width, int height, const std::string& title) :
	width(WIN_WIDTH),
	height(WIN_HEIGHT),
    old_height(WIN_HEIGHT),
    old_width(WIN_WIDTH),
	title(title),
	win_input(this),
	close_requested(false)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	sdl_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, SDL_WINDOW_OPENGL);
	gl_context = SDL_GL_CreateContext(sdl_window);

	SDL_GL_SetSwapInterval(1);


	//glewExperimental = GL_TRUE;
	
	GLenum res = glewInit();
	if(res != GLEW_OK)
	{
		fprintf(stderr, "Error setting up GLEW: '%s'\n", glewGetErrorString(res));
	}
}

window::~window()
{
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

void window::update()
{
    for(int i = 0; i < input::KEYS; i++)
    {
        win_input.set_key_up(i, false);
    }

	for(int i = 0; i < input::MOUSE_BUTTONS; i++)
	{
		win_input.set_mouse_down(i, false);
		win_input.set_mouse_up(i, false);
	}

	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_QUIT)
		{
			close_requested = true;
		}

		if(e.type == SDL_MOUSEMOTION)
		{
            win_input.set_mouse_y(e.motion.x);
            win_input.set_mouse_x(e.motion.y);
		}

		if(e.type == SDL_KEYDOWN)
		{
            win_input.set_key_down(e.key.keysym.scancode, true);
		}

		if(e.type == SDL_KEYUP)
		{
			int value = e.key.keysym.scancode;

            win_input.set_key_down(value, false);
			win_input.set_key_up(value, true);
		}

		if(e.type == SDL_MOUSEBUTTONDOWN)
		{
            win_input.set_mouse_down(e.button.button, true);
		}

		if(e.type == SDL_MOUSEBUTTONUP)
		{
			win_input.set_mouse_up(e.button.button, true);
		}
	}
}

void window::swap_buffers()
{
	SDL_GL_SwapWindow(sdl_window);
}

void window::bind_as_render_targets()
{
	glBindTexture(GL_TEXTURE_2D,0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
}

void window::set_full_screen(bool value)
{
	int mode = 0;
	if(value)
    {
        mode = SDL_WINDOW_FULLSCREEN;
        SDL_DisplayMode display_mode;
        SDL_GetCurrentDisplayMode(0, &display_mode);

        width = display_mode.w;
        height = display_mode.h;
    }
	else
    {
        mode = 0;
        width = old_width;
        height = old_height;
    }

    SDL_SetWindowSize(sdl_window, width, height);

    SDL_SetWindowFullscreen(sdl_window, mode);
}

