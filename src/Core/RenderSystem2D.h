#ifndef VMV_RENDERSYSTEM2D_H
#define VMV_RENDERSYSTEM2D_H

#include "VMVCamera.h"
#include "VMVDevice.h"
#include "VMVFrameInfo.h"
#include "VMVGameObject.h"
#include "VMVPipeline.h"

#include <memory>
#include <vector>

namespace vmv
{
	class RenderSystem2D
	{
		public:
        RenderSystem2D(VMVDevice& device, VkRenderPass renderPass);
        ~RenderSystem2D();

        RenderSystem2D(const RenderSystem2D&) = delete;
        RenderSystem2D(RenderSystem2D&&) noexcept = delete;
        RenderSystem2D& operator=(const RenderSystem2D&) = delete;
        RenderSystem2D& operator=(RenderSystem2D&&) noexcept = delete;

        void DrawGameObjects(VMVFrameInfo& frameInfo, std::vector<VMVGameObject>& gameObjects);

      private:
        struct ObjectTransformPushConstant
        {
            alignas(16) glm::mat4 model{1.f};
        };

        VMVDevice& m_VMVDevice;
        std::unique_ptr<VMVPipeline> m_pVMVPipeline;
        VkPipelineLayout m_PipelineLayout;

        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass renderPass);
	};
} // namespace vmv


#endif
