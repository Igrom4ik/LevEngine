#pragma once
#include "input/InputManager.hpp"
#include "graphics/ShaderProgram.hpp"
#include "Core/graphics/GraphicsAPI.hpp"
#include "Core/render/Render.hpp"
#include "EngineConfig.h"
#include "Core/graphics/Colors.hpp"
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

		GraphicsAPI& GetGraphicsAPI();
        RenderQueue& GetRenderQueue();


    private:
        std::unique_ptr<Application> m_application;
        std::chrono::steady_clock::time_point m_lastTimePoint;
		GLFWwindow* m_window = nullptr;

		InputManager m_inputManager;

		GraphicsAPI m_graphicsAPI;
		RenderQueue m_renderQueue;

    };
}  // namespace LEN
