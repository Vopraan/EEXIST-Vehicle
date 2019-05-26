#include "../public/EEXIST.h"
#include "../public/Window.h"

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <unistd.h>

bool isRunning();

Window* Tubes = NULL;
Window* Biases = NULL;
EEXIST* AI = NULL;

Vec2 Res = Vec2(400, 400);

int main()
{
	// CREATE AI
	AI = new EEXIST();

	// CREATE WINDOWS AND SET THEM UP
	Tubes = new Window("Tube Values", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Res.X, Res.Y);
	Biases = new Window("Bias Genetic Code", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Res.X, Res.Y);

	// LOOP WHILE WINDOWS SAY THEY ARE OPEN
	while(isRunning())
	{
		// RUN EEXIST ALGORITHYM ON TUBES
		AI->AttemptUpdate();

		// UPDATE WINDOWS WITH NEW DATA GNERATED FROM UPDATE
		Tubes->update(AI->GetAISystem(), true);
		Biases->update(AI->GetAISystem(), false);
	}
}

bool isRunning()
{
	// CHEKC FOR CLOSE WINDOW EVENT
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == 768)
		{
			return false;
		}
	}
	return true;
}
