#ifndef VMV_VMVMODEL_H
#define VMV_VMVMODEL_H

#include "VMVDevice.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vmv
{
    class VMVModel
    {
      public:
        struct Vertex
        {
            glm::vec2 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };

        VMVModel(VMVDevice& device, const std::vector<Vertex>& vertices);
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
    };
} // namespace vmv

#endif