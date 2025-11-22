#pragma once
#include <Core/eng.hpp>
#include <memory>

class Game : public LEN::Application {
public:
	bool Init() override;

	void Update(float deltaTime) override;

	void Destroy() override;

private:
	LEN::Scene *m_scene = nullptr;
};
