#include "VMVModel.h"

#include "VMVUtils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// Apparently this is the way recommended by https://en.cppreference.com/w/cpp/utility/hash
// to specify a template specialization for std::hash
namespace std
{
    template <> struct hash<vmv::VMVModel::Vertex>
    {
        size_t operator()(const vmv::VMVModel::Vertex& vertex) const
        {
            size_t seed{0};
            vmv::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
} // namespace std

vmv::VMVModel::VMVModel(VMVDevice& device, const Builder& builder) : m_VMVDevice{device}
{
    CreateVertexBuffers(builder.vertices);
    CreateIndexBuffers(builder.indices);
}

vmv::VMVModel::~VMVModel()
{
    vkDestroyBuffer(m_VMVDevice.device(), m_VertexBuffer, nullptr);
    vkFreeMemory(m_VMVDevice.device(), m_VertexBufferMemory, nullptr);

    if (m_HasIndexBuffer)
    {
        vkDestroyBuffer(m_VMVDevice.device(), m_IndexBuffer, nullptr);
        vkFreeMemory(m_VMVDevice.device(), m_IndexBufferMemory, nullptr);
    }
}

std::unique_ptr<vmv::VMVModel> vmv::VMVModel::CreateModelFromFile(VMVDevice& device, const std::string& filePath)
{
    Builder builder{};
    builder.LoadModel(filePath);

    return std::make_unique<VMVModel>(device, builder);
}

void vmv::VMVModel::Bind(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[] = {m_VertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (m_HasIndexBuffer)
    {
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }
}

void vmv::VMVModel::Draw(VkCommandBuffer commandBuffer)
{
    if (m_HasIndexBuffer)
    {
        vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
    }
    else
    {
        vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
    }
}

void vmv::VMVModel::CreateVertexBuffers(const std::vector<Vertex>& indices)
{
    m_VertexCount = static_cast<uint32_t>(indices.size());
    assert(m_VertexCount >= 3 && "Model has less than 3 indices!");

    VkDeviceSize bufferSize{sizeof(indices[0]) * m_VertexCount};

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    m_VMVDevice.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             stagingBuffer,
                             stagingBufferMemory);

    void* data;
    vkMapMemory(m_VMVDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(m_VMVDevice.device(), stagingBufferMemory);

    m_VMVDevice.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             m_VertexBuffer,
                             m_VertexBufferMemory);

    m_VMVDevice.copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

    vkDestroyBuffer(m_VMVDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(m_VMVDevice.device(), stagingBufferMemory, nullptr);
}

void vmv::VMVModel::CreateIndexBuffers(const std::vector<uint32_t>& indices)
{
    m_IndexCount = static_cast<uint32_t>(indices.size());
    m_HasIndexBuffer = m_IndexCount > 0;

    if (!m_HasIndexBuffer)
        return;

    VkDeviceSize bufferSize{sizeof(indices[0]) * m_IndexCount};
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    m_VMVDevice.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             stagingBuffer,
                             stagingBufferMemory);

    void* data;
    vkMapMemory(m_VMVDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(m_VMVDevice.device(), stagingBufferMemory);

    m_VMVDevice.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             m_IndexBuffer,
                             m_IndexBufferMemory);

    m_VMVDevice.copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

    vkDestroyBuffer(m_VMVDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(m_VMVDevice.device(), stagingBufferMemory, nullptr);
}

std::vector<VkVertexInputBindingDescription> vmv::VMVModel::Vertex::GetBindingDescriptions()
{
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> vmv::VMVModel::Vertex::GetAttributeDescriptions()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

    attributeDescriptions.push_back(
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, position))});

    attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, color))});

    attributeDescriptions.push_back(
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, normal))});

    attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, uv))});

    return attributeDescriptions;
}

void vmv::VMVModel::Builder::LoadModel(const std::string& filePath)
{
    using namespace tinyobj;
    attrib_t attrib;
    std::vector<shape_t> shapes;
    std::vector<material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const shape_t& shape : shapes)
    {
        for (const index_t& index : shape.mesh.indices)
        {
            Vertex vertex{};

            if (index.vertex_index >= 0)
            {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };

                vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2],
                };
            }
            if (index.normal_index >= 0)
            {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
            }
            if (index.texcoord_index >= 0)
            {
                vertex.uv = {attrib.texcoords[2 * index.texcoord_index + 0],
                             attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}
