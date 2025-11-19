#pragma once
#include <Core/eng.hpp>
#include <memory>

class Game : public LEN::Application
{
public:
	bool Init() override;
	void Update(float deltatime) override;
	void Destroy() override;

private:
	LEN::Material m_material;
	std::unique_ptr<LEN::Mesh> m_mesh; // Using unique_ptr for automatic memory management
};