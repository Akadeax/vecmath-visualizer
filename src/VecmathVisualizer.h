#ifndef VMV_VECMATHVISUALIZER_H
#define VMV_VECMATHVISUALIZER_H

#include "Core/VMVDevice.h"
#include "Core/VMVPipeline.h"
#include "Core/VMVSwapChain.h"
#include "Core/VMVWindow.h"
#include <memory>
#include <vector>

namespace vmv
{
    class VecmathVisualizer
    {
      public:
        VecmathVisualizer();
        ~VecmathVisualizer();

        VecmathVisualizer(const VecmathVisualizer&) = delete;
        VecmathVisualizer(VecmathVisualizer&&) noexcept = delete;
        VecmathVisualizer& operator=(const VecmathVisualizer&) = delete;
        VecmathVisualizer& operator=(VecmathVisualizer&&) noexcept = delete;

        static constexpr uint32_t WIDTH{800};
        static constexpr uint32_t HEIGHT{600};

        void Run();

      private:
        VMVWindow m_VMVWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        VMVDevice m_VMVDevice{m_VMVWindow};
        VMVSwapChain m_VMVSwapChain{m_VMVDevice, m_VMVWindow.GetExtent()};

        std::unique_ptr<VMVPipeline> m_pVMVPipeline;

        VkPipelineLayout m_PipelineLayout;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void DrawFrame();
    };
} // namespace vmv

#endif