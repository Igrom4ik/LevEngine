#include "Engine.hpp"
#include "Application.hpp"
#include "scene/Component.hpp"
#include "scene/components/CameraComponent.hpp"
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#if defined(TRACY_ENABLE)
#include <tracy/Tracy.hpp>
#endif


namespace LEN {
    Engine::Engine() = default;

    Engine::~Engine() = default;

    LEN::Key GLFWKeyToKey(int glfwKey) {
        using KEY = LEN::Key;
        if (glfwKey >= GLFW_KEY_A && glfwKey <= GLFW_KEY_Z)
            return static_cast<KEY>(static_cast<int>(KEY::A) + (glfwKey - GLFW_KEY_A));
        if (glfwKey >= GLFW_KEY_0 && glfwKey <= GLFW_KEY_9)
            return static_cast<KEY>(static_cast<int>(KEY::Num0) + (glfwKey - GLFW_KEY_0));
        switch (glfwKey) {
            case GLFW_KEY_SPACE: return KEY::Space;
            case GLFW_KEY_ESCAPE: return KEY::Escape;
            case GLFW_KEY_ENTER: return KEY::Enter;
            case GLFW_KEY_LEFT: return KEY::Left;
            case GLFW_KEY_RIGHT: return KEY::Right;
            case GLFW_KEY_UP: return KEY::Up;
            case GLFW_KEY_DOWN: return KEY::Down;
            default: return KEY::Unknown;
        }
    }

    void keyCallback(GLFWwindow * /*window*/, int key, int /*scancode*/, int action, int /*mods*/) {
        auto mapped = GLFWKeyToKey(key);
        if (mapped == LEN::Key::Unknown) return; // ignore unmapped keys

        auto &inputManager = LEN::Engine::GetInstance().GetInputManager();

        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            inputManager.SetKeyPressed(mapped, true);
        else if (action == GLFW_RELEASE)
            inputManager.SetKeyPressed(mapped, false);
    }

    void mouseButtonCallback(GLFWwindow * /*window*/, int button, int action, int /*mods*/) {
        auto &inputManager = LEN::Engine::GetInstance().GetInputManager();
        LEN::Key mapped;
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                mapped = LEN::Key::MouseLeft;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                mapped = LEN::Key::MouseRight;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                mapped = LEN::Key::MouseMiddle;
                break;
            default:
                return; // ignore unmapped buttons
        }

        // Update input manager based on action
        if (action == GLFW_PRESS) {
            inputManager.SetMouseButtonPressed(mapped, true);
        } else if (action == GLFW_RELEASE) {
            inputManager.SetMouseButtonPressed(mapped, false);
        }
    }

    void mouseCursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {
        auto &inputManager = LEN::Engine::GetInstance().GetInputManager();
        inputManager.SetMousePositionOld(inputManager.GetMousePositionCurrent());

        glm::vec2 currentPos(static_cast<float>(xpos), static_cast<float>(ypos));
        inputManager.SetMousePositionCurrent(currentPos);
    }

    Engine &Engine::GetInstance() {
        static Engine instance;
        return instance;
    }

    bool Engine::Init(int width, int height) {
        if (!m_application) {
            std::cerr << "Engine::Init(): application not set" << std::endl;
            return false;
        }

        if (!glfwInit()) {
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

        if (m_window == nullptr) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwSetKeyCallback(m_window, keyCallback);
        // Register mouse button callback to handle mouse clicks
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetCursorPosCallback(m_window, mouseCursorPositionCallback);


        /************************************************************************
         *                          INIT: GLEW                                     *
         *  - Initialize GLEW after a valid OpenGL context is current             *
         ************************************************************************/
        glfwMakeContextCurrent(m_window);

        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            glfwTerminate();
            m_window = nullptr;
            return false;
        }

        m_graphicsAPI.Init();
        return m_application->Init();
    }

    void Engine::Run() {
        if (!m_application || !m_window) return;

        m_lastTimePoint = std::chrono::high_resolution_clock::now();
        while (!glfwWindowShouldClose(m_window) && !m_application->NeedsToBeClose()) {
#if defined(TRACY_ENABLE)
            ZoneScoped; // profile whole frame
#endif
            glfwPollEvents(); // Process window events

            auto now = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(now - m_lastTimePoint).count();
            m_lastTimePoint = now;

            m_application->Update(deltaTime);

            m_graphicsAPI.SetColor(LEN::Color::BLACK, 1.0f);
            m_graphicsAPI.ClearBuffers();

            CameraData cameraData{};

            int width(0), height(0);
            glfwGetWindowSize(m_window, &width, &height);
            float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

            if (m_currentScene) {
                if (auto cameraObject = m_currentScene->GetMainCamera()) {
                    // Logic for matrices
                    auto cameraComponent = cameraObject->GetComponent<CameraComponent>();
                    if (cameraComponent) {
                        cameraData.viewMatrix = cameraComponent->GetViewMatrix();
                        cameraData.projectionMatrix = cameraComponent->GetProjectionMatrix(aspectRatio);
                    }
                }
            }
            m_renderQueue.Draw(m_graphicsAPI, cameraData);

            glfwSwapBuffers(m_window); // Swap front and back buffers

            m_inputManager.SetMousePositionOld(m_inputManager.GetMousePositionCurrent()); // Update old mouse position
#if defined(TRACY_ENABLE)
            FrameMark; // mark end of frame
#endif
        }
    }


    void Engine::Destroy() {
        if (m_application) {
            m_application->Destroy();
            m_application.reset();
            if (m_window) {
                glfwDestroyWindow(m_window);
                m_window = nullptr;
            }
            glfwTerminate();
        }
    }

    void Engine::SetApplication(Application *app) {
        m_application.reset(app);
    }

    Application *Engine::GetApplication() {
        return m_application.get();
    }

    InputManager &Engine::GetInputManager() {
        return m_inputManager;
    }


    GraphicsAPI &Engine::GetGraphicsAPI() {
        return m_graphicsAPI;
    }

    RenderQueue &Engine::GetRenderQueue() {
        return m_renderQueue;
    }

    FileSystem &Engine::GetFileSystem() {
        return m_fileSystem;
    }

    void Engine::SetScene(Scene *scene) {
        m_currentScene.reset(scene);
    }

    Scene *Engine::GetCurrentScene() {
        return m_currentScene.get();
    }
} // namespace LEN
