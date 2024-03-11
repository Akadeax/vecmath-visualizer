#ifndef VMV_VMVMODEL_H
#define VMV_VMVMODEL_H

#include "VMVDevice.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vmv
{
    class VMVModel final
    {
      public:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };

        struct Builder
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };

        VMVModel(VMVDevice& device, const Builder& builder);
        ~VMVModel();

        VMVModel(const VMVModel&) = delete;
        VMVModel(VMVModel&&) noexcept = delete;
        VMVModel& operator=(const VMVModel&) = delete;
        VMVModel& operator=(VMVModel&&) noexcept = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

      private:
        VMVDevice& m_VMVDevice;

        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        uint32_t m_VertexCount;

        void CreateVertexBuffers(const std::vector<Vertex>& vertices);

        bool m_HasIndexBuffer{false};
        VkBuffer m_IndexBuffer;
        VkDeviceMemory m_IndexBufferMemory;
        uint32_t m_IndexCount;

        void CreateIndexBuffers(const std::vector<uint32_t>& indices);
    };
} // namespace vmv

#endif