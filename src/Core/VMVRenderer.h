#ifndef VMV_VMVRENDERER_H
#define VMV_VMVRENDERER_H

#include "VMVDevice.h"
#include "VMVModel.h"
#include "VMVSwapChain.h"
#include "VMVWindow.h"
#include <cassert>
#include <memory>
#include <vector>

namespace vmv
{
    class VMVRenderer
    {
      public:
        VMVRenderer(VMVWindow& window, VMVDevice& device);
        ~VMVRenderer();

        VMVRenderer(const VMVRenderer&) = delete;
        VMVRenderer(VMVRenderer&&) noexcept = delete;
        VMVRenderer& operator=(const VMVRenderer&) = delete;
        VMVRenderer& operator=(VMVRenderer&&) noexcept = delete;

        bool IsFrameInProgress() const { return m_IsFrameStarted; }
        VkCommandBuffer GetCurrentCommandBuffer() const;

        VkRenderPass GetSwapChainRenderPass() const { return m_pVMVSwapChain->getRenderPass(); }
        int GetFrameIndex() const;
        float GetAspectRatio() const { return m_pVMVSwapChain->extentAspectRatio(); }

        VkCommandBuffer BeginFrame();
        void EndFrame();

        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

      private:
        VMVWindow& m_VMVWindow;
        VMVDevice& m_VMVDevice;
        std::unique_ptr<VMVSwapChain> m_pVMVSwapChain;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        uint32_t m_CurrentImageIndex;
        uint32_t m_CurrentFrameIndex{};

        bool m_IsFrameStarted{false};

        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapChain();
    };
} // namespace vmv

#endif