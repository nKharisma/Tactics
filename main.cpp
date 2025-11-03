#include "game/game.h"
#include <iostream>

int main(int argc, char* argv[]) {
	Game game;
	
	if (!game.Initialize())
	{
		std::cerr << "Failed to initialize the game." << std::endl;
		return -1;
	}
	
	game.Run();
	
	game.Cleanup();
	
	return 0;
}