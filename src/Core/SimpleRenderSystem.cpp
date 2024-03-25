#include "SimpleRenderSystem.h"
#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "VMVSwapChain.h"
#include <glm/glm.hpp>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

vmv::SimpleRenderSystem::SimpleRenderSystem(VMVDevice& device, VkRenderPass renderPass) : m_VMVDevice{device}
{
    CreateDescriptorSetLayout();

    CreatePipelineLayout();
    CreatePipeline(renderPass);

    CreateUniformBuffers();

    CreateDescriptorPool();
    CreateDescriptorSets();
}

vmv::SimpleRenderSystem::~SimpleRenderSystem()
{
    vkDestroyPipelineLayout(m_VMVDevice.device(), m_PipelineLayout, nullptr);
    vkDestroyDescriptorPool(m_VMVDevice.device(), m_DescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(m_VMVDevice.device(), m_DescriptorSetLayout, nullptr);
}

void vmv::SimpleRenderSystem::CreatePipelineLayout()
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ObjectTransformPushConstant);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(m_VMVDevice.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create pipeline layout!"};
    }
}

void vmv::SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
{
    assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

    PipelineConfigInfo pipelineConfig{};
    VMVPipeline::DefaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = m_PipelineLayout;
    m_pVMVPipeline = std::make_unique<VMVPipeline>(
        m_VMVDevice, pipelineConfig, "Shaders/simple_shader.vert.spv", "Shaders/simple_shader.frag.spv");
}

void vmv::SimpleRenderSystem::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding globalUboBinding{};
    globalUboBinding.binding = 0;
    globalUboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    globalUboBinding.descriptorCount = 1;
    globalUboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    globalUboBinding.pImmutableSamplers = nullptr; // for image sampling

    std::vector<VkDescriptorSetLayoutBinding> bindings{globalUboBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(m_VMVDevice.device(), &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create descriptor set layout!"};
    }
}

void vmv::SimpleRenderSystem::CreateUniformBuffers()
{
    m_GlobalUboBuffers.resize(VMVSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i{}; i < m_GlobalUboBuffers.size(); ++i)
    {
        m_GlobalUboBuffers[i] =
            std::make_unique<VMVBuffer>(m_VMVDevice,
                                        sizeof(GlobalUbo),
                                        1,
                                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        m_VMVDevice.properties.limits.minUniformBufferOffsetAlignment);
        m_GlobalUboBuffers[i]->map();
    }
}

void vmv::SimpleRenderSystem::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(VMVSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(VMVSwapChain::MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(m_VMVDevice.device(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create Descriptor Pool!"};
    }
}

void vmv::SimpleRenderSystem::CreateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts{VMVSwapChain::MAX_FRAMES_IN_FLIGHT, m_DescriptorSetLayout};
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(VMVSwapChain::MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    m_DescriptorSets.resize(VMVSwapChain::MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(m_VMVDevice.device(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to allocate Descriptor Sets!"};
    }

    for (size_t i{}; i < m_DescriptorSets.size(); ++i)
    {
        // This DescriptorInfo is for buffers, for images use VkDescriptorImageInfo
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_GlobalUboBuffers[i]->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(GlobalUbo);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_DescriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        // descriptorWrite.pImageInfo = nullptr; // If this was an image buffer use this to set the texture

        vkUpdateDescriptorSets(m_VMVDevice.device(), 1, &descriptorWrite, 0, nullptr);
    }
}

void vmv::SimpleRenderSystem::DrawGameObjects(VMVFrameInfo& frameInfo, std::vector<VMVGameObject>& gameObjects)
{
    m_pVMVPipeline->Bind(frameInfo.commandBuffer);

    // glm::mat4 projectionView{frameInfo.camera.GetProjection() * frameInfo.camera.GetView()};

    UpdateGlobalUbo(frameInfo);
    vkCmdBindDescriptorSets(frameInfo.commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_PipelineLayout,
                            0,
                            1,
                            &m_DescriptorSets[frameInfo.frameIndex],
                            0,
                            nullptr);

    for (VMVGameObject& go : gameObjects)
    {
        ObjectTransformPushConstant push{};
        push.model = go.m_Transform.GetMat();
        push.normalMatrix = go.m_Transform.NormalMatrix();

        vkCmdPushConstants(frameInfo.commandBuffer,
                           m_PipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(ObjectTransformPushConstant),
                           &push);

        go.m_Model->Bind(frameInfo.commandBuffer);
        go.m_Model->Draw(frameInfo.commandBuffer);
    }
}

void vmv::SimpleRenderSystem::UpdateGlobalUbo(const VMVFrameInfo& frameInfo)
{
    GlobalUbo ubo{};
    ubo.view = frameInfo.camera.GetView();
    ubo.proj = frameInfo.camera.GetProjection();

    m_GlobalUboBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo, VK_WHOLE_SIZE);
    m_GlobalUboBuffers[frameInfo.frameIndex]->flush(VK_WHOLE_SIZE); // Prolly don't have to flush?
}
