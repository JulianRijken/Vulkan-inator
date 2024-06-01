#pragma once

#include <vulkan/vulkan_core.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <array>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <memory>
#include <vector>

#include "Buffer.h"

class Material;
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
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 tangent;
        glm::vec2 uv;

        static const VkVertexInputBindingDescription BINDING_DESCRIPTION;
        static const std::array<VkVertexInputAttributeDescription, 4> ATTRIBUTE_DESCRIPTIONS;

        bool operator==(const Vertex3D& other) const
        {
            return position == other.position and normal == other.normal and tangent == other.tangent and
                   uv == other.uv;
        }
    };

    struct VertexData
    {
        void* data;
        uint32_t vertexCount;
        uint32_t typeSize;
    };

    Mesh(const std::vector<uint32_t>& indicies, const VertexData& vertexData, Material* material);

    void Draw(VkCommandBuffer commandBuffer) const;

    [[nodiscard]] Material* GetMaterial() const { return m_MaterialPtr; }

    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);  // Trivial set and get

private:
    std::unique_ptr<Buffer> m_StagingBuffer;
    std::unique_ptr<Buffer> m_VertexBuffer;
    std::unique_ptr<Buffer> m_IndexBuffer;

    Material* m_MaterialPtr;

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

            return (hash(vert.position) ^ hash(vert.tangent) << 1) >> 1 ^ hash(vert.normal) << 1 ^
                   hash(glm::vec3(vert.uv, 1.0f));
        }
    };
};  // namespace std
