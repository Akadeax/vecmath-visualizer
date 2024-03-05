#include "VecmathVisualizer.h"
#include <stdexcept>

vmv::VecmathVisualizer::VecmathVisualizer()
{
    CreatePipelineLayout();
    CreatePipeline();
    CreateCommandBuffers();
}

vmv::VecmathVisualizer::~VecmathVisualizer()
{
    vkDestroyPipelineLayout(m_VMVDevice.device(), m_PipelineLayout, nullptr);
}

void vmv::VecmathVisualizer::Run()
{
    while (!m_VMVWindow.ShouldClose())
    {
        glfwPollEvents();
    }
}

void vmv::VecmathVisualizer::CreatePipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(m_VMVDevice.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create pipeline layout!"};
    }
}

void vmv::VecmathVisualizer::CreatePipeline()
{
    PipelineConfigInfo pipelineConfig{
        VMVPipeline::DefaultPipelineConfigInfo(m_VMVSwapChain.width(), m_VMVSwapChain.height())};

    pipelineConfig.renderPass = m_VMVSwapChain.getRenderPass();
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    m_pVMVPipeline = std::make_unique<VMVPipeline>(
        m_VMVDevice, pipelineConfig, "Shaders/simple_shader.vert.spv", "Shaders/simple_shader.frag.spv");
}

void vmv::VecmathVisualizer::CreateCommandBuffers()
{
}

void vmv::VecmathVisualizer::DrawFrame()
{
}
