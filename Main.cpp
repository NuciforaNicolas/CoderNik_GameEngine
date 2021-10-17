#include <iostream>
#include "Game.h"

#define WIDTH 1280
#define HEIGHT 720

int main(int argc, char* argv[]) {
	// Create a Game
	Game game;
	// Set width and height of the game window
	game.SetWindowWidthHeight(WIDTH, HEIGHT);
	// Initialize the Game
	bool isGameInitialized = game.Initialize();
	if (isGameInitialized) {
		// If succeeded, run the loop
		game.RunLoop();
	}
	// if any error or Player quit, shutdown the game
	game.ShutDown();
	return 0;
}