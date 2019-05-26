#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <stdio.h>

class Window
{
private:

	int WIDTH = 0;
	int HEIGHT = 0;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

public:

	Window(const char* title, int xpos, int ypos, int width, int height);

	void update(struct System* Sys, bool bUseTube);
};

#endif // WINDOW_H
