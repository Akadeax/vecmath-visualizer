#ifndef VMV_VMVMODEL_H
#define VMV_VMVMODEL_H

#include "VMVBuffer.h"
#include "VMVDevice.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>

namespace vmv
{
    class VMVModel final
    {
      public:
        struct Vertex
        {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

            bool operator==(const Vertex& other) const
            {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };

        struct Builder
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void LoadModel(const std::string& filePath);
        };

        VMVModel(VMVDevice& device, const Builder& builder);
        ~VMVModel();

        VMVModel(const VMVModel&) = delete;
        VMVModel(VMVModel&&) noexcept = delete;
        VMVModel& operator=(const VMVModel&) = delete;
        VMVModel& operator=(VMVModel&&) noexcept = delete;

        static std::unique_ptr<VMVModel> CreateModelFromFile(VMVDevice& device, const std::string& filePath);

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

      private:
        VMVDevice& m_VMVDevice;

        std::unique_ptr<VMVBuffer> m_VertexBuffer;
        uint32_t m_VertexCount;

        void CreateVertexBuffers(const std::vector<Vertex>& vertices);

        bool m_HasIndexBuffer{false};
        std::unique_ptr<VMVBuffer> m_IndexBuffer;
        uint32_t m_IndexCount;

        void CreateIndexBuffers(const std::vector<uint32_t>& indices);
    };
} // namespace vmv

#endif