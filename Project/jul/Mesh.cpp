#include "Mesh.h"

#include "vulkanbase/VulkanUtil.h"

Mesh::Mesh(const std::vector<uint32_t>& indicies, const VertexData& vertexData) :
    m_NumIndices{ static_cast<uint32_t>(indicies.size()) }
{
    const VkDeviceSize vertexBufferSize{ vertexData.typeSize * vertexData.vertexCount };
    const VkDeviceSize indicesBufferSize{ indicies.size() * sizeof(uint32_t) };


    m_StagingBuffer =
        std::make_unique<Buffer>(std::max(vertexBufferSize, indicesBufferSize),
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

        );


    m_StagingBuffer->Upload(vertexData.data, (size_t)vertexBufferSize);
    m_VertexBuffer = std::make_unique<Buffer>(vertexBufferSize,
                                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

    );


    vulkanUtil::CopyBuffer(*m_StagingBuffer, *m_VertexBuffer, vertexBufferSize);


    m_StagingBuffer->Upload((void*)indicies.data(), indicesBufferSize);
    m_IndexBuffer = std::make_unique<Buffer>(indicesBufferSize,
                                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

    );

    vulkanUtil::CopyBuffer(*m_StagingBuffer, *m_IndexBuffer, indicesBufferSize);
}

void Mesh::Draw(VkCommandBuffer commandBuffer) const
{
    VkBuffer vertexBuffers[] = { *m_VertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, *m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, m_NumIndices, 1, 0, 0, 0);
}

const VkVertexInputBindingDescription Mesh::Vertex2D::BINDING_DESCRIPTION{

    .binding = 0, .stride = sizeof(Mesh::Vertex2D), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};

const std::array<VkVertexInputAttributeDescription, 2> Mesh::Vertex2D::ATTRIBUTE_DESCRIPTIONS{

    VkVertexInputAttributeDescription{
                                      .location = 0,
                                      .binding = 0,
                                      .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex2D, position),
                                      },
    { .location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Vertex2D, color) }
};


const VkVertexInputBindingDescription Mesh::Vertex3D::BINDING_DESCRIPTION{ .binding = 0,
                                                                           .stride = sizeof(Mesh::Vertex3D),
                                                                           .inputRate = VK_VERTEX_INPUT_RATE_VERTEX };

const std::array<VkVertexInputAttributeDescription, 4> Mesh::Vertex3D::ATTRIBUTE_DESCRIPTIONS{
    VkVertexInputAttributeDescription{
                                      .location = 0,
                                      .binding = 0,
                                      .format = VK_FORMAT_R32G32B32_SFLOAT,
                                      .offset = offsetof(Mesh::Vertex3D, pos),
                                      },
    VkVertexInputAttributeDescription{
                                      .location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Mesh::Vertex3D, normal) },
    VkVertexInputAttributeDescription{
                                      .location = 2, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Mesh::Vertex3D, color) },
    VkVertexInputAttributeDescription{
                                      .location = 3, .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT, .offset = offsetof(Mesh::Vertex3D, texCoord) }
};
