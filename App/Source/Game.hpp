#pragma once

#include <Core/eng.hpp>

class Game : public LEN::Application
{
public:
	bool Init() override;
	void Update(float deltatime) override;
	void Destroy() override;


};