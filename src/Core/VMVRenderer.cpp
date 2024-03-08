#include "VMVRenderer.h"

#include <array>
#include <stdexcept>

vmv::VMVRenderer::VMVRenderer(VMVWindow& window, VMVDevice& device) : m_VMVWindow{window}, m_VMVDevice{device}
{
    RecreateSwapChain();
    CreateCommandBuffers();
}

vmv::VMVRenderer::~VMVRenderer()
{
    FreeCommandBuffers();
}

void vmv::VMVRenderer::FreeCommandBuffers()
{
    vkFreeCommandBuffers(m_VMVDevice.device(),
                         m_VMVDevice.getCommandPool(),
                         static_cast<uint32_t>(m_CommandBuffers.size()),
                         m_CommandBuffers.data());

    m_CommandBuffers.clear();
}

void vmv::VMVRenderer::CreateCommandBuffers()
{
    m_CommandBuffers.resize(VMVSwapChain::MAX_FRAMES_IN_FLIGHT);

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

void vmv::VMVRenderer::RecreateSwapChain()
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
        std::shared_ptr<VMVSwapChain> oldSwapChain{std::move(m_pVMVSwapChain)};
        m_pVMVSwapChain = std::make_unique<VMVSwapChain>(m_VMVDevice, extent, oldSwapChain);

        if (!oldSwapChain->CompareSwapFormats(*m_pVMVSwapChain.get()))
        {
            throw std::runtime_error("Swap chain image or depth format has changed!");
        }
    }

    // If render pass compatible do nothing else
    // CreatePipeline(); TODO
}

VkCommandBuffer vmv::VMVRenderer::GetCurrentCommandBuffer() const
{
    assert(m_IsFrameStarted && "Frame is not in progress!");
    return m_CommandBuffers[m_CurrentFrameIndex];
}

int vmv::VMVRenderer::GetFrameIndex() const
{
    assert(m_IsFrameStarted && "Cannot get frame index when frame is not in progress!");
    return m_CurrentFrameIndex;
}

VkCommandBuffer vmv::VMVRenderer::BeginFrame()
{
    assert(!m_IsFrameStarted && "Cannot begin frame while frame is in progress!");
    VkResult result{m_pVMVSwapChain->acquireNextImage(&m_CurrentImageIndex)};

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error{"Failed to aquire next swapchain image!"};
    }

    m_IsFrameStarted = true;

    VkCommandBuffer commandBuffer{GetCurrentCommandBuffer()};

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to begin recording command buffer!"};
    }

    return commandBuffer;
}

void vmv::VMVRenderer::EndFrame()
{
    assert(m_IsFrameStarted && "Cannot end frame while not in progress!");

    VkCommandBuffer commandBuffer{GetCurrentCommandBuffer()};
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to record command buffer!"};
    }

    VkResult result{m_pVMVSwapChain->submitCommandBuffers(&commandBuffer, &m_CurrentImageIndex)};

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_VMVWindow.WasWindowResized())
    {
        m_VMVWindow.ResetWindowResizedFlag();
        RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to submit command buffers!"};
    }

    m_IsFrameStarted = false;
    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % VMVSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void vmv::VMVRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(m_IsFrameStarted && "Cannot call BeginSwapChainRenderPass if frame is not in progress!");
    assert(commandBuffer == GetCurrentCommandBuffer() &&
           "Cannot begin render pass on command buffer from a different frame!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_pVMVSwapChain->getRenderPass();
    renderPassInfo.framebuffer = m_pVMVSwapChain->getFrameBuffer(m_CurrentImageIndex);

    renderPassInfo.renderArea.offset = VkOffset2D{0, 0};
    renderPassInfo.renderArea.extent = m_pVMVSwapChain->getSwapChainExtent();

    constexpr VkClearColorValue clearColor{0.01f, 0.01f, 0.01f, 1.0f};
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = VkClearDepthStencilValue{1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_pVMVSwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(m_pVMVSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, m_pVMVSwapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void vmv::VMVRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(m_IsFrameStarted && "Cannot call EndSwapChainRenderPass if frame is not in progress!");
    assert(commandBuffer == GetCurrentCommandBuffer() &&
           "Cannot end render pass on command buffer from a different frame!");

    vkCmdEndRenderPass(commandBuffer);
}
