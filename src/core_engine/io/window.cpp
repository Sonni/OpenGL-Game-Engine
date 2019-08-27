#include "window.hpp"
#include <GL/glew.h>

window::window(int width, int height, const std::string& title) :
		m_width(width),
		m_height(height),
		m_old_height(WIN_HEIGHT),
		m_old_width(WIN_WIDTH),
		m_title(title),
		m_win_input(this),
		m_close_requested(false)
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

	m_sdl_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_OPENGL);
	m_gl_context = SDL_GL_CreateContext(m_sdl_window);

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
	SDL_GL_DeleteContext(m_gl_context);
	SDL_DestroyWindow(m_sdl_window);
	SDL_Quit();
}

void window::update()
{
    for(int i = 0; i < input::KEYS; i++)
    {
		m_win_input.set_key_up(i, false);
    }

	for(int i = 0; i < input::MOUSE_BUTTONS; i++)
	{
		m_win_input.set_mouse_down(i, false);
		m_win_input.set_mouse_up(i, false);
	}

	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_QUIT)
		{
			m_close_requested = true;
		}

		if(e.type == SDL_MOUSEMOTION)
		{
			m_win_input.set_mouse_y(e.motion.x);
			m_win_input.set_mouse_x(e.motion.y);
		}

		if(e.type == SDL_KEYDOWN)
		{
			m_win_input.set_key_down(e.key.keysym.scancode, true);
		}

		if(e.type == SDL_KEYUP)
		{
			int value = e.key.keysym.scancode;

			m_win_input.set_key_down(value, false);
			m_win_input.set_key_up(value, true);
		}

		if(e.type == SDL_MOUSEBUTTONDOWN)
		{
			m_win_input.set_mouse_down(e.button.button, true);
		}

		if(e.type == SDL_MOUSEBUTTONUP)
		{
			m_win_input.set_mouse_up(e.button.button, true);
		}
	}
}

void window::swap_buffers()
{
	SDL_GL_SwapWindow(m_sdl_window);
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

		m_width = display_mode.w;
		m_height = display_mode.h;
    }
	else
    {
        mode = 0;
		m_width = m_old_width;
		m_height = m_old_height;
    }

    SDL_SetWindowSize(m_sdl_window, m_width, m_height);

    SDL_SetWindowFullscreen(m_sdl_window, mode);
}

