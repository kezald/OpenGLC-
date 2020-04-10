#include "Errors.h"

#include <cstdlib>
#include <iostream>
#include <SDL/SDL.h>

void fatalError(std::string errStr) {
	std::cout << errStr << std::endl;
	std::cout << "Press key to quit...";
	int tmp;
	std::cin >> tmp;
	SDL_Quit(); //This shuts down SDL
	exit(1); //This exits the game
}