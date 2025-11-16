// ============================================================================
// main.cpp - LEN Application Entry Point
// ============================================================================
#include "Game.hpp"
#include "Core/eng.hpp"


int main() {

	Game* game = new Game();
	LEN::Engine engine;
	engine.SetApplication(game);

	engine.Run();
	engine.Destroy();

	return EXIT_SUCCESS;
}