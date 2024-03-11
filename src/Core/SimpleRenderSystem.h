#ifndef VMV_SimpleRenderSystem_H
#define VMV_SimpleRenderSystem_H

#include "VMVCamera.h"
#include "VMVDevice.h"
#include "VMVGameObject.h"
#include "VMVPipeline.h"

#include <memory>
#include <vector>

namespace vmv
{
    class SimpleRenderSystem
    {
      public:
        SimpleRenderSystem(VMVDevice& device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem(SimpleRenderSystem&&) noexcept = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(SimpleRenderSystem&&) noexcept = delete;

        void DrawGameObjects(VkCommandBuffer commandBuffer,
                             std::vector<VMVGameObject>& gameObjects,
                             const VMVCamera& camera);

      private:
        struct SimplePushConstantData
        {
            glm::mat4 transform{1.f};
            glm::mat4 normalMatrix{1.f};
        };

        VMVDevice& m_VMVDevice;

        std::unique_ptr<VMVPipeline> m_pVMVPipeline;
        VkPipelineLayout m_PipelineLayout;

        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass renderPass);
    };
} // namespace vmv

#endif