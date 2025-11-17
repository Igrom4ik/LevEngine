#pragma once
#include "Input/InputManager.hpp"
#include "EngineConfig.h"
#include <memory>
#include <chrono>

struct GLFWwindow;

namespace LEN
{
    inline Key GLFWKeyToKey(int glfwKey);

    class Application;
    class Engine
    {
    public:
 
        ~Engine();

		static Engine& GetInstance();

    private:
        Engine();
        Engine(const Engine&) = delete;
        Engine(Engine&&) = delete;
        Engine& operator = (const Engine&) = delete;
        Engine& operator = (Engine&&) = delete;

    public:

        bool Init(int width, int height);
        void Run();
        void Destroy();

        void SetApplication(Application* app);
        Application* GetApplication();

		InputManager& GetInputManager();

    private:
        std::unique_ptr<Application> m_application;
        std::chrono::steady_clock::time_point m_lastTimePoint;
		GLFWwindow* m_window = nullptr;

		InputManager m_inputManager;
    };
}  // namespace LEN
