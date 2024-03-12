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

        [[nodiscard]] static VkVertexInputBindingDescription GetBindingDescriptor();
        [[nodiscard]] static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
    };

    Mesh(VkDevice device, std::vector<Vertex> verts, VkPhysicalDevice physicalDevice);
    ~Mesh();

    void Draw(VkCommandBuffer commandBuf) const;

private:

    VkDevice m_Device;
    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;

    uint32_t m_NumVerts;
};
