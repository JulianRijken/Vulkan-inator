#pragma once

#include <vulkan/vulkan_core.h>

#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Buffer.h"

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

    void Draw(VkCommandBuffer commandBuffer) const;

private:
    std::unique_ptr<Buffer> m_StagingBuffer;
    std::unique_ptr<Buffer> m_VertexBuffer;
    std::unique_ptr<Buffer> m_IndexBuffer;

    VkDevice m_Device;

    uint32_t m_NumIndices;
};
