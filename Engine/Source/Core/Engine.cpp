#include "Engine.hpp"
#include "Application.hpp"
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace LEN
{
    Engine::Engine() = default;
    Engine::~Engine() = default;

    bool Engine::Init(int width, int height)
    {
        if (!m_application)
        {
            std::cerr << "Engine::Init(): application not set" << std::endl;
            return false;
        }

        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        /************************************************************************
         *            WINDOW HINTS / OPENGL CONTEXT                               *
         *  - Request an OpenGL 3.3 Core Profile context                           *
         ************************************************************************/
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        /************************************************************************
         *                          CREATE WINDOW                                 *
         *  - Create the application window and exit if creation fails            *
         ************************************************************************/
        m_window = glfwCreateWindow(width, height, "LEN", nullptr, nullptr);

        if (m_window == nullptr)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }


        /************************************************************************
         *                          INIT: GLEW                                     *
         *  - Initialize GLEW after a valid OpenGL context is current             *
         ************************************************************************/
        glfwMakeContextCurrent(m_window);

        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            glfwTerminate();
            m_window = nullptr;
            return false;
        }

        return m_application->Init();
    }

    void Engine::Run()
    {
        
        if (!m_application || !m_window) return;

        m_lastTimePoint = std::chrono::high_resolution_clock::now();
        while (!glfwWindowShouldClose(m_window) && !m_application->NeedsToBeClose())
        {
            glfwPollEvents(); // Process window events

            auto now = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(now - m_lastTimePoint).count();
            m_lastTimePoint = now;

            m_application->Update(deltaTime);

            glfwSwapBuffers(m_window); // Swap front and back buffers
        }
    }


    void Engine::Destroy()
    {
        if (m_application)
        {
            m_application->Destroy();
            m_application.reset();
            if (m_window)
            {
                glfwDestroyWindow(m_window);
                m_window = nullptr;
            }
            glfwTerminate();
        }
    }

    void Engine::SetApplication(Application* app)
    {
        m_application.reset(app);
    }

    Application* Engine::GetApplication()
    {
        return m_application.get();
    }
} // namespace LEN
