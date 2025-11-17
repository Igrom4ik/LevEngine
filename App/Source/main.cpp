// ============================================================================
// main.cpp - LEN Application Entry Point
// ============================================================================
#include "Game.hpp"
#include "Core/eng.hpp"


int main() {

	Game* game = new Game();
	LEN::Engine& engine = LEN::Engine::GetInstance();
	engine.SetApplication(game);

	if (!engine.Init(1280, 720)) {
		return EXIT_FAILURE;
	}

	engine.Run();
	engine.Destroy();
}