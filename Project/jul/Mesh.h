#pragma once

#include <vulkan/vulkan_core.h>

#include <array>
#include <glm/glm.hpp>
#include <vector>

class Mesh final
{
public:

    struct Vertex2D
    {
        glm::vec2 pos;
        glm::vec3 color;

        static const VkVertexInputBindingDescription BINDING_DESCRIPTION;
        static const std::array<VkVertexInputAttributeDescription, 2> ATTRIBUTE_DESCRIPTIONS;
    };


    struct Vertex3D
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec3 color;

        static const VkVertexInputBindingDescription BINDING_DESCRIPTION;
        static const std::array<VkVertexInputAttributeDescription, 3> ATTRIBUTE_DESCRIPTIONS;
    };

    struct VertexData
    {
        void* data;
        uint32_t vertexCount;
        uint32_t typeSize;
    };

    Mesh(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue, const std::vector<uint32_t>& indicies,
         const VertexData& vertexData);
    ~Mesh();

    Mesh(Mesh&&) noexcept;
    Mesh(const Mesh &) = delete;
    Mesh &operator=(Mesh &&) = delete;
    Mesh &operator=(const Mesh &) = delete;


    void Draw(VkCommandBuffer commandBuf) const;

private:

    VkDevice m_Device;
    VkBuffer m_VertexBuffer{};
    VkDeviceMemory m_VertexBufferMemory{};

    VkBuffer m_IndexBuffer{};
    VkDeviceMemory m_IndexBufferMemory{};


    VkBuffer m_StagingBuffer{};
    VkDeviceMemory m_StagingBufferMemory{};

    uint32_t m_NumIndices;
};
