#include "VecmathVisualizer.h"
#include <array>
#include <stdexcept>

#include "Core/SimpleRenderSystem.h"
#include "Core/VMVModel.h"

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
    VMVCamera camera{};

    while (!m_VMVWindow.ShouldClose())
    {
        glfwPollEvents();

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

// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<vmv::VMVModel> createCubeModel(vmv::VMVDevice& device, glm::vec3 offset)
{
    std::vector<vmv::VMVModel::Vertex> vertices{

        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

    };
    for (auto& v : vertices)
    {
        v.position += offset;
    }
    return std::make_unique<vmv::VMVModel>(device, vertices);
}

void vmv::VecmathVisualizer::LoadGameObjects()
{
    std::shared_ptr<VMVModel> cubeModel{createCubeModel(m_VMVDevice, {0.f, 0.f, 0.f})};
    VMVGameObject cubeObject{VMVGameObject::CreateGameObject()};
    cubeObject.m_Model = cubeModel;
    cubeObject.m_Transform.translation = {0.f, 0.f, 7.5f};
    cubeObject.m_Transform.scale = {0.5f, 0.5f, 0.5f};

    m_GameObjects.push_back(std::move(cubeObject));
}
