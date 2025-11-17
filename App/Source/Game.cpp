#include "Game.hpp"
#include <iostream>

bool Game::Init()
{
	return true;
}

void Game::Update(float deltatime)
{
	auto& input = LEN::Engine::GetInstance().GetInputManager();

	if (input.IsKeyPressed(LEN::Key::A))
	{
		std::cout << "[A] button is pressed" << std::endl;
	}
}

void Game::Destroy()
{
	
}
