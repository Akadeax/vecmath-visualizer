#include "VecmathVisualizer.h"
#include <array>
#include <stdexcept>

vmv::VecmathVisualizer::VecmathVisualizer()
{
    LoadModels();

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
        DrawFrame();
    }

    vkDeviceWaitIdle(m_VMVDevice.device());
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
    m_CommandBuffers.resize(m_VMVSwapChain.imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_VMVDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    if (vkAllocateCommandBuffers(m_VMVDevice.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to allocate command buffers!"};
    }

    for (int i{0}; i < m_CommandBuffers.size(); ++i)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error{"Failed to begin recording command buffer!"};
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_VMVSwapChain.getRenderPass();
        renderPassInfo.framebuffer = m_VMVSwapChain.getFrameBuffer(i);

        renderPassInfo.renderArea.offset = VkOffset2D{0, 0};
        renderPassInfo.renderArea.extent = m_VMVSwapChain.getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = VkClearColorValue{0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = VkClearDepthStencilValue{1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        m_pVMVPipeline->Bind(m_CommandBuffers[i]);

        m_VMVModel->Bind(m_CommandBuffers[i]);
        m_VMVModel->Draw(m_CommandBuffers[i]);

        vkCmdEndRenderPass(m_CommandBuffers[i]);

        if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error{"Failed to record command buffer!"};
        }
    }
}

void vmv::VecmathVisualizer::DrawFrame()
{
    uint32_t imageIndex;
    VkResult result{m_VMVSwapChain.acquireNextImage(&imageIndex)};

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to aquire next swapchain image!"};
    }

    result = m_VMVSwapChain.submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to submit command buffers!"};
    }
}

void vmv::VecmathVisualizer::LoadModels()
{
    std::vector<VMVModel::Vertex> vertices{VMVModel::Vertex{glm::vec2(0.f, -0.5f)},
                                           VMVModel::Vertex{glm::vec2(0.5f, 0.5f)},
                                           VMVModel::Vertex{glm::vec2(-0.5f, 0.5f)}};

    m_VMVModel = std::make_unique<VMVModel>(m_VMVDevice, vertices);
}
