#pragma once
#include <Core/eng.hpp>
#include <Core/render/Material.hpp>

class Game : public LEN::Application
{
public:
	bool Init() override;
	void Update(float deltatime) override;
	void Destroy() override;

private:
	LEN::Material m_material;
};