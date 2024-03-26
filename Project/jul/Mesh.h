#pragma once

#include <glm/glm.hpp>
#include <array>
#include <vulkan/vulkan_core.h>

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

    Mesh(VkDevice device,const VertexData& vertexData, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue);
    ~Mesh();

    void Draw(VkCommandBuffer commandBuf) const;

private:

    VkDevice m_Device;
    VkBuffer m_VertexBuffer{};
    VkDeviceMemory m_VertexBufferMemory{};

    VkBuffer m_StagingBuffer{};
    VkDeviceMemory m_StagingBufferMemory{};

    uint32_t m_NumVerts;
};
