#pragma once

#include <Core/eng.hpp>
#include <memory>

class TestObject : public LEN::GameObject
{
public:
	TestObject();
	void Update(float deltaTime) override;

private:

};