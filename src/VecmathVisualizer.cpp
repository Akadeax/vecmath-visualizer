#include "VecmathVisualizer.h"
#include <array>
#include <chrono>
#include <stdexcept>

#include "Core/SimpleRenderSystem.h"
#include "Core/RenderSystem2D.h"
#include "Core/VMVBuffer.h"
#include "Core/VMVFrameInfo.h"
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
    RenderSystem2D renderSystem2D{m_VMVDevice, m_VMVRenderer.GetSwapChainRenderPass()};
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
            int frameIndex{m_VMVRenderer.GetFrameIndex()};
            VMVFrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera};

            // render
            m_VMVRenderer.BeginSwapChainRenderPass(commandBuffer);
            renderSystem2D.DrawGameObjects(frameInfo, m_GameObjects2D);
            renderSystem.DrawGameObjects(frameInfo, m_GameObjects);
            m_VMVRenderer.EndSwapChainRenderPass(commandBuffer);

            m_VMVRenderer.EndFrame();
        }
    }

    vkDeviceWaitIdle(m_VMVDevice.device());
}

void vmv::VecmathVisualizer::LoadGameObjects()
{
    std::shared_ptr<VMVModel> model{VMVModel::CreateModelFromFile(m_VMVDevice, "data/models/flat_vase.obj")};
    VMVGameObject gameObject{VMVGameObject::CreateGameObject()};
    gameObject.m_Model = model;
    gameObject.m_Transform.translation = {-0.5f, 0.5f, 2.5f};
    gameObject.m_Transform.scale = {3.f, 3.f, 3.f};

    m_GameObjects.push_back(std::move(gameObject));

    std::shared_ptr<VMVModel> model2{VMVModel::CreateModelFromFile(m_VMVDevice, "data/models/smooth_vase.obj")};
    VMVGameObject gameObject2{VMVGameObject::CreateGameObject()};
    gameObject2.m_Model = model2;
    gameObject2.m_Transform.translation = {0.5f, 0.5f, 2.5f};
    gameObject2.m_Transform.scale = {2.f, 2.f, 2.f};

    m_GameObjects.push_back(std::move(gameObject2));


    std::shared_ptr<VMVModel> model3{VMVModel::CreateModelFromFile(m_VMVDevice, "data/models/smooth_vase.obj")};
    VMVGameObject gameObject3{VMVGameObject::CreateGameObject()};
    gameObject3.m_Model = model3;
    gameObject3.m_Transform.translation = {0.8f, 0.8f, 0.f};
    gameObject3.m_Transform.scale = {2.f, 2.f, 2.f};

    m_GameObjects2D.push_back(std::move(gameObject3));
}
