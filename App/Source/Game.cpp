#include "Game.hpp"
#include <iostream>

bool Game::Init()
{
	return true;
}

void Game::Update(float deltatime)
{
	std::cout << "Deltatime: " << deltatime << " seconds" << std::endl;
}

void Game::Destroy()
{

}
