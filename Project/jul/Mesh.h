#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <vulkan/vulkan_core.h>

class Mesh final
{
public:

    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;

        static const VkVertexInputBindingDescription BINDING_DESCRIPTION;
        static const std::array<VkVertexInputAttributeDescription, 2> ATTRIBUTE_DESCRIPTIONS;
    };

    Mesh(VkDevice device, std::vector<Vertex> verts, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue);
    ~Mesh();

    void Draw(VkCommandBuffer commandBuf) const;

private:

    VkDevice m_Device;
    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;

    VkBuffer m_StagingBuffer;
    VkDeviceMemory m_StagingBufferMemory;

    uint32_t m_NumVerts;
};
