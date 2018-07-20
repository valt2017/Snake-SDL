#include <iostream>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

// Ported by @valt to SDL from https://www.youtube.com/user/FamTrinli snake tutorial in SFML
enum {EMPTY = 0, FOOD, SNAKE};
const int sizeItem = 16;
const int ItemsX = 30, ItemsY = 20;
const int sizeX = ItemsX*sizeItem, sizeY = ItemsY*sizeItem;
const int FPS = 60;
const int ANIMATION_DELAY = 5000;
SDL_Rect board = { 0, 0, sizeX, sizeY };
Uint32 start;

struct Food
{
	SDL_Rect rect;

	Food() {
		newPosition();
		rect.h = sizeItem;
		rect.w = sizeItem;
	}

	void newPosition() {
		rect.x = (rand() % ItemsX)*sizeItem;
		rect.y = (rand() % ItemsY)*sizeItem;
	}
};

struct Snake
{
	SDL_Rect rect = {0, 0, sizeItem, sizeItem};
	int direction = 1;

	void move()
	{
		if (direction == 0 && rect.y > board.y) {
			rect.y -= sizeItem;
		} 
		if (direction == 1 && rect.x < board.w - sizeItem) {
			rect.x += sizeItem;
		} 
		if (direction == 2 && rect.y < board.h - sizeItem) {
			rect.y += sizeItem;
		}
		if (direction == 3 && rect.x > board.x) {
			rect.x -= sizeItem;
		}
	}

	SDL_bool eatFood(Food & food) {
		return SDL_HasIntersection(&rect, &food.rect);
	}
};


int main(int argc, char ** argv) {
	SDL_Window *win = nullptr;
	SDL_Renderer *renderer = nullptr;
	// Error checks
	std::cout << "SDL_Init\n";
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("Snake", 100, 100, sizeX, sizeY, 0);
	if (win == nullptr) {
		std::cout << "SDL_CreateWindow error\n";
	}
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cout << "SDL_CreateRenderer error\n";
	}
	// Load bitmaps
	std::vector<std::string> Bitmaps;
	Bitmaps.push_back("img/white.png");
	Bitmaps.push_back("img/red.png");
	Bitmaps.push_back("img/green.png");
	// Create textures from bitmaps
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cout << "SDL_image could not initialize! SDL_image Error:" << IMG_GetError() << std::endl;
	}
	IMG_Init(IMG_INIT_PNG);
	std::vector<SDL_Texture *> Textures;
	for (auto bitmap : Bitmaps) {
		Textures.push_back(IMG_LoadTexture(renderer, bitmap.c_str()));
		if (Textures.back() == nullptr) {
			std::cout << bitmap.c_str() << " SDL_CreateTextureFromSurface error\n";
		}
		else
			std::cout << bitmap.c_str() << " SDL_CreateTextureFromSurface OK\n";
	}
	std::vector<Snake> snake;
	snake.push_back(Snake());
	Food food;

	while (1) {
		start = SDL_GetTicks();
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
			/* Look for a keypress */
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_LEFT:
					snake.front().direction = 3;
					break;
				case SDLK_RIGHT:
					snake.front().direction = 1;
					break;
				case SDLK_UP:
					snake.front().direction = 0;
					break;
				case SDLK_DOWN:
					snake.front().direction = 2;
					break;
				default:
					std::cout << "Key not supported" << std::endl;
					break;
				}
			}
		}
		SDL_RenderClear(renderer);
		// board texture
		auto texture = Textures[EMPTY];
		for (int j = 0; j < ItemsY; j++) {
			for (int i = 0; i < ItemsX; i++) {
				SDL_Rect item = { i*sizeItem, j*sizeItem, sizeItem, sizeItem };
				SDL_RenderCopy(renderer, texture, nullptr, &item);
			}
		}
		// snake texture
		texture = Textures[SNAKE];
		for(int i = 0; i < snake.size(); i++)
			SDL_RenderCopy(renderer, texture, nullptr, &snake[i].rect);
		// food texture
		texture = Textures[FOOD];
		SDL_RenderCopy(renderer, texture, nullptr, &food.rect);
		SDL_RenderPresent(renderer);
		// move all snake items to new positions
		for (int i = 0; i < snake.size(); i++)
			snake[i].move();
		// update direction of each snake item according previous item (except the first)
		for (int i = snake.size() - 1; i > 0; i--)
			snake[i].direction = snake[i - 1].direction;
		// head of the snake eats the food ?
		if (snake[0].eatFood(food)) {
			// copy the last item of the snake
			snake.push_back(snake.back());
			// dont't move the added item yet (two at the same position) -> direction set to 5
			snake.back().direction = 5;
			food.newPosition();
		}
		// Animation delay
		if (ANIMATION_DELAY / FPS > SDL_GetTicks() - start)
			SDL_Delay(ANIMATION_DELAY / FPS - (SDL_GetTicks() - start));
		// End of game?
		if (snake.size() >= 10)
			break;
	}
	IMG_Quit();
	for (auto texture : Textures)
		SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	std::cout << "SDL_Quit\n";
	return 0;
}