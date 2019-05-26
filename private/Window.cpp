#include "../public/Window.h"
#include "../public/EEXIST.h"

// INIT SDL WINDOWS FOR AI DISPLAY
Window::Window(const char* title, int xpos, int ypos, int width, int height)
{
	// SAVE WIDTH AND HEIGHT OF WINDOW
	WIDTH = width;
	HEIGHT = height;

	// INIT SDL FOR WINDOW
	if(!SDL_Init(SDL_INIT_EVERYTHING) == 0) { printf("SDL FAILED\n"); exit(0); }

	// CREATE WINDOW
	window = SDL_CreateWindow(title, xpos, ypos, HEIGHT, HEIGHT, 0);
	if(window == NULL) { printf("SDL WINDOW FAILED\n"); exit(0); }

	// CREATE RENDERER
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(renderer == NULL) { printf("SDL RENDERER FAILED\n"); exit(0); }
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

// UPDATE WINDOW WITH GIVEN DATA
void Window::update(struct System* CurSystem, bool bUseTube)
{
	// CLEAR THE SCREEN FROM OLD DATA
	SDL_RenderClear(renderer);
	

	// LOOP THROUGH EACH PIXEL/TUBE ON THE SCREEN
	for(int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			// CLACUALTE DIEMNTSIONS FOR PIXEL DRAWING
			struct SDL_Rect Rect;
			Rect.h = HEIGHT / SIZE;
			Rect.w = WIDTH / SIZE;
			Rect.x = (WIDTH / SIZE) * x;
			Rect.y = (HEIGHT / SIZE) * y;

			// WILL HOLD VALUES FOR EACH TUBE
			int R, G, B, A;


			if(bUseTube)
			{
				// CALCAULATE COLORS FOR TUBES
				R = (CurSystem->Tubes[x][y].SrcX / SIZE) * 255;
				G = (CurSystem->Tubes[x][y].SrcY / SIZE) * 255;
				B = (CurSystem->Tubes[x][y].DesX / SIZE) * 255;
				A = (CurSystem->Tubes[x][y].DesY / SIZE) * 255;
			}
			else
			{
				// CALCUALTYE COLORS FOR BIAS
				R = ((float)CurSystem->Biases[x][y].SrcX / (float)SIZE) * 255;
				G = ((float)CurSystem->Biases[x][y].SrcY / (float)SIZE) * 255;
				B = ((float)CurSystem->Biases[x][y].DesX / (float)SIZE) * 255;
				A = ((float)CurSystem->Biases[x][y].DesY / (float)SIZE) * 255;
			}

			// DRAW CALCAUTED PIXEL TO RENDERER
			SDL_SetRenderDrawColor(renderer, R, G, B, A);
			SDL_RenderDrawRect(renderer, &Rect);
			SDL_RenderFillRect(renderer, &Rect);
		}
	}

	// PRESENT RENDERER TO WINDOW
	SDL_RenderPresent(renderer);
}
