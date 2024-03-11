#include "VecmathVisualizer.h"
#include <array>
#include <chrono>
#include <stdexcept>

#include "Core/SimpleRenderSystem.h"
#include "Core/VMVModel.h"
#include "KeyboardMovementController.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

vmv::VecmathVisualizer::VecmathVisualizer()
{
    LoadGameObjects();
}

vmv::VecmathVisualizer::~VecmathVisualizer() {}

void vmv::VecmathVisualizer::Run()
{
    SimpleRenderSystem renderSystem{m_VMVDevice, m_VMVRenderer.GetSwapChainRenderPass()};

    VMVGameObject viewer{VMVGameObject::CreateGameObject()};
    VMVCamera camera{};

    KeyboardMovementController input{};

    using namespace std::chrono;
    time_point currentTime{high_resolution_clock::now()};

    while (!m_VMVWindow.ShouldClose())
    {
        glfwPollEvents();
        time_point newTime{high_resolution_clock::now()};
        float frameTime{duration<float, seconds::period>(newTime - currentTime).count()};
        currentTime = newTime;

        input.MoveInPlaneXZ(m_VMVWindow.GetWindow(), frameTime, viewer);
        camera.SetViewEuler(viewer.m_Transform.translation, viewer.m_Transform.rotation);

        float aspect{m_VMVRenderer.GetAspectRatio()};
        // camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
        camera.SetPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);

        if (VkCommandBuffer commandBuffer{m_VMVRenderer.BeginFrame()})
        {
            m_VMVRenderer.BeginSwapChainRenderPass(commandBuffer);
            renderSystem.DrawGameObjects(commandBuffer, m_GameObjects, camera);
            m_VMVRenderer.EndSwapChainRenderPass(commandBuffer);

            m_VMVRenderer.EndFrame();
        }
    }

    vkDeviceWaitIdle(m_VMVDevice.device());
}

// temporary helper function, creates a 1x1x1 cube centered at offset with an index buffer
std::unique_ptr<vmv::VMVModel> createCubeModel(vmv::VMVDevice& device, glm::vec3 offset)
{
    vmv::VMVModel::Builder modelBuilder{};
    modelBuilder.vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };
    for (auto& v : modelBuilder.vertices)
    {
        v.position += offset;
    }

    modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                            12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

    return std::make_unique<vmv::VMVModel>(device, modelBuilder);
}

void vmv::VecmathVisualizer::LoadGameObjects()
{
    std::shared_ptr<VMVModel> cubeModel{createCubeModel(m_VMVDevice, {0.f, 0.f, 0.f})};
    VMVGameObject cubeObject{VMVGameObject::CreateGameObject()};
    cubeObject.m_Model = cubeModel;
    cubeObject.m_Transform.translation = {0.f, 0.f, 2.5f};
    cubeObject.m_Transform.scale = {1.f, 1.f, 1.f};

    m_GameObjects.push_back(std::move(cubeObject));
}
