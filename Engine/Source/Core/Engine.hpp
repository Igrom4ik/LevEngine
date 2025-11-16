#pragma once
#include "EngineConfig.h"
#include <memory>
#include <chrono>

struct GLFWwindow;

namespace LEN
{
    class Application;
    class Engine
    {
    public:
        Engine();
        ~Engine();

        bool Init(int width, int height);
        void Run();
        void Destroy();

        void SetApplication(Application* app);
        Application* GetApplication();

    private:
        std::unique_ptr<Application> m_application;
        std::chrono::steady_clock::time_point m_lastTimePoint;
		GLFWwindow* m_window = nullptr;
    };
}  // namespace LEN
