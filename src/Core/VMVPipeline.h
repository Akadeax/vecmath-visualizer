#ifndef VMV_VMVPIPELINE_H
#define VMV_VMVPIPELINE_H

#include "VMVDevice.h"
#include "VMVModel.h"

#include <string>
#include <vector>

namespace vmv
{
    struct PipelineConfigInfo final
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;

        VkPipelineLayout pipelineLayout{nullptr};
        VkRenderPass renderPass{nullptr};
        uint32_t subpass{0};
    };

    class VMVPipeline final
    {
      public:
        VMVPipeline(VMVDevice& device,
                    const PipelineConfigInfo& configInfo,
                    const std::string& vertFilePath,
                    const std::string& fragFilePath);

        ~VMVPipeline();
        VMVPipeline(const VMVPipeline&) = delete;
        VMVPipeline(VMVPipeline&&) noexcept = delete;
        VMVPipeline& operator=(const VMVPipeline&) = delete;
        VMVPipeline& operator=(VMVPipeline&&) noexcept = delete;

        static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

        void Bind(VkCommandBuffer commandBuffer);

      private:
        static std::vector<char> ReadFile(const std::string& filePath);

        void CreateGraphicsPipeline(const PipelineConfigInfo& configInfo,
                                    const std::string& vertFilePath,
                                    const std::string& fragFilePath);

        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        VMVDevice& m_VMVDevice;
        VkPipeline m_GraphicsPipeline;
        VkShaderModule m_VertShaderModule;
        VkShaderModule m_FragShaderModule;
    };
} // namespace vmv

#endif
