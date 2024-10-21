
/*
 * Another World engine rewrite
 * Copyright (C) 2004-2005 Gregory Montoir (cyx@users.sourceforge.net)
 */

#include <SDL.h>
#include "graphics.h"
#include "systemstub.h"
#include "util.h"

struct SystemStub_Dummy : SystemStub {

	static const int kJoystickIndex = 0;
	static const int kJoystickCommitValue = 16384;
	static const float kAspectRatio;

	int _w, _h;
	float _aspectRatio[4];
	SDL_Window *_window;
	SDL_Renderer *_renderer;
	SDL_GLContext _glcontext;
	int _texW, _texH;
	SDL_Texture *_texture;
	SDL_Joystick *_joystick;
	SDL_GameController *_controller;
	int _screenshot;

	SystemStub_Dummy();
	virtual ~SystemStub_Dummy() {}

	virtual void init(const char *title, const DisplayMode *dm);
	virtual void fini();

	virtual void prepareScreen(int &w, int &h, float ar[4]);
	virtual void updateScreen();
	virtual void setScreenPixels555(const uint16_t *data, int w, int h);

	virtual void processEvents();
	virtual void sleep(uint32_t duration);
	virtual uint32_t getTimeStamp();

	void setAspectRatio(int w, int h);
};

const float SystemStub_Dummy::kAspectRatio = 16.f / 10.f;

SystemStub_Dummy::SystemStub_Dummy()
	: _w(0), _h(0), _window(0), _renderer(0), _texW(0), _texH(0), _texture(0) {
}

void SystemStub_Dummy::init(const char *title, const DisplayMode *dm) {
	// SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	// SDL_ShowCursor(SDL_DISABLE);
	// SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	// int windowW = 0;
	// int windowH = 0;
	int flags = dm->opengl ? SDL_WINDOW_OPENGL : 0;
	if (dm->mode == DisplayMode::WINDOWED) {
		flags |= SDL_WINDOW_RESIZABLE;
		// windowW = dm->width;
		// windowH = dm->height;
	} else {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	// _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowW, windowH, flags);
	// SDL_GetWindowSize(_window, &_w, &_h);

	if (dm->opengl) {
		// _glcontext = SDL_GL_CreateContext(_window);
	} else {
		_glcontext = 0;
		// _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
		// SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
		// SDL_RenderClear(_renderer);
	}
	_aspectRatio[0] = _aspectRatio[1] = 0.;
	_aspectRatio[2] = _aspectRatio[3] = 1.;
	if (dm->mode == DisplayMode::FULLSCREEN_AR) {
		if (dm->opengl) {
			// setAspectRatio(_w, _h);
		} else {
			// SDL_RenderSetLogicalSize(_renderer, 320, 200);
		}
	}
	_joystick = 0;
	_controller = 0;
	_screenshot = 1;
	_dm = *dm;
}

void SystemStub_Dummy::fini() {
	// if (_texture) {
	// 	// SDL_DestroyTexture(_texture);
	// 	_texture = 0;
	// }
	// if (_joystick) {
	// 	SDL_JoystickClose(_joystick);
	// 	_joystick = 0;
	// }
	// if (_controller) {
	// 	SDL_GameControllerClose(_controller);
	// 	_controller = 0;
	// }
	// if (_renderer) {
	// 	// SDL_DestroyRenderer(_renderer);
	// 	_renderer = 0;
	// }
	// if (_glcontext) {
	// 	SDL_GL_DeleteContext(_glcontext);
	// 	_glcontext = 0;
	// }
	// // SDL_DestroyWindow(_window);
	// SDL_Quit();
}

void SystemStub_Dummy::prepareScreen(int &w, int &h, float ar[4]) {
}

void SystemStub_Dummy::updateScreen() {
}

void SystemStub_Dummy::setScreenPixels555(const uint16_t *data, int w, int h) {
}

void SystemStub_Dummy::processEvents() {
}

void SystemStub_Dummy::sleep(uint32_t duration) {
	// SDL_Delay(duration);
}

uint32_t SystemStub_Dummy::getTimeStamp() {
	return SDL_GetTicks();
}

void SystemStub_Dummy::setAspectRatio(int w, int h) {
}

SystemStub *SystemStub_Dummy_create() {
	return new SystemStub_Dummy();
}


thread_local SystemStub *dummyStub ;//= System_SDL_create();