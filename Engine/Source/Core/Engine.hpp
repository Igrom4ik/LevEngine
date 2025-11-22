#pragma once
#include "input/InputManager.hpp"
#include "graphics/ShaderProgram.hpp"
#include "Core/graphics/GraphicsAPI.hpp"
#include "Core/render/RenderQueue.hpp"
#include "Core/scene/Scene.hpp"
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

        void SetScene(Scene* scene);
        Scene* GetCurrentScene();

    private:
        std::unique_ptr<Application> m_application;
        std::chrono::steady_clock::time_point m_lastTimePoint;
		GLFWwindow* m_window = nullptr;

		InputManager m_inputManager;

		GraphicsAPI m_graphicsAPI;
		RenderQueue m_renderQueue;

        std::unique_ptr<Scene> m_currentScene;

    };
}  // namespace LEN
