#include "VecmathVisualizer.h"
#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

vmv::VecmathVisualizer::VecmathVisualizer()
{
    LoadModels();

    CreatePipelineLayout();
    RecreateSwapChain();
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

void vmv::VecmathVisualizer::CreatePipeline()
{
    assert(m_pVMVSwapChain != nullptr && "Cannot create pipeline before swap chain!");
    assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

    PipelineConfigInfo pipelineConfig{};
    VMVPipeline::DefaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = m_pVMVSwapChain->getRenderPass();
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    m_pVMVPipeline = std::make_unique<VMVPipeline>(
        m_VMVDevice, pipelineConfig, "Shaders/simple_shader.vert.spv", "Shaders/simple_shader.frag.spv");
}

void vmv::VecmathVisualizer::FreeCommandBuffers()
{
    vkFreeCommandBuffers(m_VMVDevice.device(),
                         m_VMVDevice.getCommandPool(),
                         static_cast<uint32_t>(m_CommandBuffers.size()),
                         m_CommandBuffers.data());

    m_CommandBuffers.clear();
}

void vmv::VecmathVisualizer::CreateCommandBuffers()
{
    m_CommandBuffers.resize(m_pVMVSwapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_VMVDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    if (vkAllocateCommandBuffers(m_VMVDevice.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to allocate command buffers!"};
    }
}

void vmv::VecmathVisualizer::DrawFrame()
{
    uint32_t imageIndex{};
    VkResult result{m_pVMVSwapChain->acquireNextImage(&imageIndex)};

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error{"Failed to aquire next swapchain image!"};
    }

    RecordCommandBuffer(imageIndex);
    result = m_pVMVSwapChain->submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_VMVWindow.WasWindowResized())
    {
        m_VMVWindow.ResetWindowResizedFlag();
        RecreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to submit command buffers!"};
    }
}

void vmv::VecmathVisualizer::RecreateSwapChain()
{
    VkExtent2D extent{m_VMVWindow.GetExtent()};
    while (extent.width == 0 || extent.height == 0)
    {
        extent = m_VMVWindow.GetExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_VMVDevice.device());

    if (m_pVMVSwapChain == nullptr)
    {
        m_pVMVSwapChain = std::make_unique<VMVSwapChain>(m_VMVDevice, extent);
    }
    else
    {
        m_pVMVSwapChain = std::make_unique<VMVSwapChain>(m_VMVDevice, extent, std::move(m_pVMVSwapChain));
        if (m_pVMVSwapChain->imageCount() != m_CommandBuffers.size())
        {
            FreeCommandBuffers();
            CreateCommandBuffers();
        }
    }

    // If render pass compatible do nothing else
    CreatePipeline();
}

void vmv::VecmathVisualizer::RecordCommandBuffer(int imageIndex)
{
    static int frame{0};
    frame = (frame + 1) % 1000;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to begin recording command buffer!"};
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_pVMVSwapChain->getRenderPass();
    renderPassInfo.framebuffer = m_pVMVSwapChain->getFrameBuffer(imageIndex);

    renderPassInfo.renderArea.offset = VkOffset2D{0, 0};
    renderPassInfo.renderArea.extent = m_pVMVSwapChain->getSwapChainExtent();

    constexpr VkClearColorValue clearColor{0.01f, 0.01f, 0.01f, 1.0f};
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = VkClearDepthStencilValue{1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_pVMVSwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(m_pVMVSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, m_pVMVSwapChain->getSwapChainExtent()};
    vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

    m_pVMVPipeline->Bind(m_CommandBuffers[imageIndex]);

    m_VMVModel->Bind(m_CommandBuffers[imageIndex]);

    for (int j{0}; j < 4; ++j)
    {
        SimplePushConstantData push{};
        push.offset = glm::vec2(-0.5f + frame * 0.002f, -0.4f + j * 0.25f);
        push.color = glm::vec3(0.f, 0.f, 0.2f + 0.2f * j);

        vkCmdPushConstants(m_CommandBuffers[imageIndex],
                           m_PipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(SimplePushConstantData),
                           &push);

        m_VMVModel->Draw(m_CommandBuffers[imageIndex]);
    }

    vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);

    if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to record command buffer!"};
    }
}

void vmv::VecmathVisualizer::LoadModels()
{
    std::vector<VMVModel::Vertex> vertices{VMVModel::Vertex{glm::vec2(0.f, -0.5f), glm::vec3(1.f, 0.f, 0.f)},
                                           VMVModel::Vertex{glm::vec2(0.5f, 0.5f), glm::vec3(0.f, 1.f, 0.f)},
                                           VMVModel::Vertex{glm::vec2(-0.5f, 0.5f), glm::vec3(0.f, 0.f, 1.f)}};

    m_VMVModel = std::make_unique<VMVModel>(m_VMVDevice, vertices);
}
