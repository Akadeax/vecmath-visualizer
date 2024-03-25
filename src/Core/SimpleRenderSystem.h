#ifndef VMV_SIMPLERENDERSYSTEM_H
#define VMV_SIMPLERENDERSYSTEM_H

#include "VMVCamera.h"
#include "VMVDevice.h"
#include "VMVFrameInfo.h"
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

        void DrawGameObjects(VMVFrameInfo& frameInfo, std::vector<VMVGameObject>& gameObjects);

      private:
        struct GlobalUbo // explicit because vec4 requires 4N (16byte) alignment
        {
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };
        struct ObjectTransformPushConstant
        {
            alignas(16) glm::mat4 model{1.f};        // for mvp
            alignas(16) glm::mat4 normalMatrix{1.f}; // for normal transformation
        };

        VMVDevice& m_VMVDevice;

        std::unique_ptr<VMVPipeline> m_pVMVPipeline;

        VkDescriptorSetLayout m_DescriptorSetLayout;
        VkPipelineLayout m_PipelineLayout;

        std::vector<std::unique_ptr<VMVBuffer>> m_GlobalUboBuffers{};

        VkDescriptorPool m_DescriptorPool;
        std::vector<VkDescriptorSet> m_DescriptorSets;

        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass renderPass);

        void CreateDescriptorSetLayout();
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();

        void UpdateGlobalUbo(const VMVFrameInfo& frameInfo);
    };
} // namespace vmv

#endif
