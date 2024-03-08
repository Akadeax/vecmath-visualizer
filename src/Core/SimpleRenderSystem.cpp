#include "SimpleRenderSystem.h"
#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

vmv::SimpleRenderSystem::SimpleRenderSystem(VMVDevice& device, VkRenderPass renderPass) : m_VMVDevice{device}
{
    CreatePipelineLayout();
    CreatePipeline(renderPass);
}

vmv::SimpleRenderSystem::~SimpleRenderSystem()
{
    vkDestroyPipelineLayout(m_VMVDevice.device(), m_PipelineLayout, nullptr);
}

void vmv::SimpleRenderSystem::CreatePipelineLayout()
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(m_VMVDevice.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create pipeline layout!"};
    }
}

void vmv::SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
{
    assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

    PipelineConfigInfo pipelineConfig{};
    VMVPipeline::DefaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    m_pVMVPipeline = std::make_unique<VMVPipeline>(
        m_VMVDevice, pipelineConfig, "Shaders/simple_shader.vert.spv", "Shaders/simple_shader.frag.spv");
}

void vmv::SimpleRenderSystem::DrawGameObjects(VkCommandBuffer commandBuffer,
                                              std::vector<VMVGameObject>& gameObjects,
                                              const VMVCamera& camera)
{
    for (VMVGameObject& go : gameObjects)
    {
        go.m_Transform.rotation.y += 0.001f;
        go.m_Transform.rotation.x += 0.0005f;
    }

    m_pVMVPipeline->Bind(commandBuffer);

    for (VMVGameObject& go : gameObjects)
    {
        SimplePushConstantData push{};
        push.transform = camera.GetProjection() * go.m_Transform.GetMat();
        push.color = go.m_Color;

        vkCmdPushConstants(commandBuffer,
                           m_PipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(SimplePushConstantData),
                           &push);

        go.m_Model->Bind(commandBuffer);
        go.m_Model->Draw(commandBuffer);
    }
}
