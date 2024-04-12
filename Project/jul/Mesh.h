#pragma once

#include <vulkan/vulkan_core.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <memory>
#include <vector>

#include "Buffer.h"

class Mesh final
{
public:

    struct Vertex2D
    {
        glm::vec2 position;
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

        bool operator==(const Vertex3D& other) const
        {
            return pos == other.pos and normal == other.normal and color == other.color;
        }
    };

    struct VertexData
    {
        void* data;
        uint32_t vertexCount;
        uint32_t typeSize;
    };

    Mesh(const std::vector<uint32_t>& indicies, const VertexData& vertexData);

    void Draw(VkCommandBuffer commandBuffer) const;

private:
    std::unique_ptr<Buffer> m_StagingBuffer;
    std::unique_ptr<Buffer> m_VertexBuffer;
    std::unique_ptr<Buffer> m_IndexBuffer;

    uint32_t m_NumIndices;
};

namespace std
{
    template<>
    struct hash<Mesh::Vertex3D>
    {
        size_t operator()(const Mesh::Vertex3D& vert) const
        {
            auto&& hash{ std::hash<glm::vec3>() };

            return (hash(vert.pos) ^ hash(vert.color) << 1) >> 1 ^ hash(vert.normal) << 1;
        }
    };
};  // namespace std
