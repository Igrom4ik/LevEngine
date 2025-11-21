#include "Game.hpp"
#include "TestObject.hpp"


bool Game::Init()
{
	m_scene.CreateObject<TestObject>("TestObject");
    return true;
}

void Game::Update(float deltaTime)
{
	m_scene.Update(deltaTime);
}

void Game::Destroy()
{
	
}
