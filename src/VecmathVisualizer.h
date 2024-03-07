#ifndef VMV_VECMATHVISUALIZER_H
#define VMV_VECMATHVISUALIZER_H

#include "Core/VMVDevice.h"
#include "Core/VMVModel.h"
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
        struct SimplePushConstantData
        {
            alignas(16) glm::vec3 color;
            alignas(8) glm::vec2 offset;
        };

        VMVWindow m_VMVWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        VMVDevice m_VMVDevice{m_VMVWindow};
        std::unique_ptr<VMVSwapChain> m_pVMVSwapChain;
        std::unique_ptr<VMVPipeline> m_pVMVPipeline;
        VkPipelineLayout m_PipelineLayout;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        std::unique_ptr<VMVModel> m_VMVModel;

        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void DrawFrame();
        void RecreateSwapChain();
        void RecordCommandBuffer(int imageIndex);

        void LoadModels();
    };
} // namespace vmv

#endif