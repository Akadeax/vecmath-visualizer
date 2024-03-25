#include "RenderSystem2D.h"
#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "VMVSwapChain.h"
#include <glm/glm.hpp>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

vmv::RenderSystem2D::RenderSystem2D(VMVDevice& device, VkRenderPass renderPass) : m_VMVDevice{device}
{
    CreatePipelineLayout();
    CreatePipeline(renderPass);
}

vmv::RenderSystem2D::~RenderSystem2D()
{
    vkDestroyPipelineLayout(m_VMVDevice.device(), m_PipelineLayout, nullptr);
}

void vmv::RenderSystem2D::CreatePipelineLayout()
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ObjectTransformPushConstant);

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

void vmv::RenderSystem2D::CreatePipeline(VkRenderPass renderPass)
{
    assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

    PipelineConfigInfo pipelineConfig{};
    VMVPipeline::DefaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    m_pVMVPipeline = std::make_unique<VMVPipeline>(
        m_VMVDevice, pipelineConfig, "Shaders/shader_2D.vert.spv", "Shaders/shader_2D.frag.spv");
}

void vmv::RenderSystem2D::DrawGameObjects(VMVFrameInfo& frameInfo, std::vector<VMVGameObject>& gameObjects)
{
    m_pVMVPipeline->Bind(frameInfo.commandBuffer);

    for (VMVGameObject& go : gameObjects)
    {
        ObjectTransformPushConstant push{};
        push.model = go.m_Transform.GetMat();

        vkCmdPushConstants(frameInfo.commandBuffer,
                           m_PipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(ObjectTransformPushConstant),
                           &push);

        go.m_Model->Bind(frameInfo.commandBuffer);
        go.m_Model->Draw(frameInfo.commandBuffer);
    }
}
