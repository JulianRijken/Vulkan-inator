#include "Mesh.h"

#include "vulkanbase/VulkanUtil.h"

Mesh::Mesh(VkDevice device, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue,
           const std::vector<uint32_t>& indicies, const VertexData& vertexData) :
    m_Device{ device },
    m_NumIndices{ static_cast<uint32_t>(indicies.size()) }
{
    const VkDeviceSize vertexBufferSize{ vertexData.typeSize * vertexData.vertexCount };
    const VkDeviceSize indicesBufferSize{ indicies.size() * sizeof(uint32_t) };


    // Create staging buffer
    std::tie(m_StagingBuffer, m_StagingBufferMemory) =
        VkUtils::CreateBuffer(std::max(vertexBufferSize, indicesBufferSize),
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              device,
                              physicalDevice);


    // Fill staging buffer with vertexData
    {
        void* data = nullptr;
        vkMapMemory(device, m_StagingBufferMemory, 0, vertexBufferSize, 0, &data);
        memcpy(data, vertexData.data, (size_t)vertexBufferSize);
        vkUnmapMemory(device, m_StagingBufferMemory);

        // Create Vertex buffer
        std::tie(m_VertexBuffer, m_VertexBufferMemory) =
            VkUtils::CreateBuffer(vertexBufferSize,
                                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                  device,
                                  physicalDevice);

        VkUtils::CopyBuffer(m_StagingBuffer, m_VertexBuffer, vertexBufferSize, device, graphicsQueue);
    }

    {
        // Fill staging buffer with indeciesData
        void* data = nullptr;
        vkMapMemory(device, m_StagingBufferMemory, 0, indicesBufferSize, 0, &data);
        memcpy(data, indicies.data(), indicesBufferSize);
        vkUnmapMemory(device, m_StagingBufferMemory);


        // Create Index Buffer
        std::tie(m_IndexBuffer, m_IndexBufferMemory) =
            VkUtils::CreateBuffer(indicesBufferSize,
                                  VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                  device,
                                  physicalDevice);

        VkUtils::CopyBuffer(m_StagingBuffer, m_IndexBuffer, indicesBufferSize, device, graphicsQueue);
    }
}

Mesh::Mesh(Mesh&& other) noexcept :
    m_Device(other.m_Device),
    m_NumIndices(other.m_NumIndices),
    m_StagingBuffer(other.m_StagingBuffer),
    m_StagingBufferMemory(other.m_StagingBufferMemory),
    m_VertexBuffer(other.m_VertexBuffer),
    m_VertexBufferMemory(other.m_VertexBufferMemory),
    m_IndexBuffer(other.m_IndexBuffer),
    m_IndexBufferMemory(other.m_IndexBufferMemory)
{
    other.m_StagingBuffer = VK_NULL_HANDLE;
    other.m_StagingBufferMemory = VK_NULL_HANDLE;
    other.m_VertexBuffer = VK_NULL_HANDLE;
    other.m_VertexBufferMemory = VK_NULL_HANDLE;
    other.m_IndexBuffer = VK_NULL_HANDLE;
    other.m_IndexBufferMemory = VK_NULL_HANDLE;
}

Mesh::~Mesh()
{
    if(m_StagingBuffer != VK_NULL_HANDLE)
        vkDestroyBuffer(m_Device, m_StagingBuffer, nullptr);

    if(m_StagingBufferMemory != VK_NULL_HANDLE)
        vkFreeMemory(m_Device, m_StagingBufferMemory, nullptr);

    if(m_VertexBuffer != VK_NULL_HANDLE)
        vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);

    if(m_VertexBufferMemory != VK_NULL_HANDLE)
        vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);

    if(m_IndexBuffer != VK_NULL_HANDLE)
        vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);

    if(m_IndexBufferMemory != VK_NULL_HANDLE)
        vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);
}

void Mesh::Draw(VkCommandBuffer commandBuf) const
{
    VkBuffer vertexBuffers[] = { m_VertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuf, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuf, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuf, m_NumIndices, 1, 0, 0, 0);
}

const VkVertexInputBindingDescription Mesh::Vertex2D::BINDING_DESCRIPTION{

    .binding = 0, .stride = sizeof(Mesh::Vertex2D), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};

const std::array<VkVertexInputAttributeDescription, 2> Mesh::Vertex2D::ATTRIBUTE_DESCRIPTIONS{

    VkVertexInputAttributeDescription{
                                      .location = 0,
                                      .binding = 0,
                                      .format = VK_FORMAT_R32G32_SFLOAT,
                                      .offset = offsetof(Vertex2D, pos),
                                      },
    { .location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Vertex2D, color) }
};


const VkVertexInputBindingDescription Mesh::Vertex3D::BINDING_DESCRIPTION{ .binding = 0,
                                                                           .stride = sizeof(Mesh::Vertex3D),
                                                                           .inputRate = VK_VERTEX_INPUT_RATE_VERTEX };

const std::array<VkVertexInputAttributeDescription, 3> Mesh::Vertex3D::ATTRIBUTE_DESCRIPTIONS{
    VkVertexInputAttributeDescription{
                                      .location = 0,
                                      .binding = 0,
                                      .format = VK_FORMAT_R32G32B32_SFLOAT,
                                      .offset = offsetof(Mesh::Vertex3D, pos),
                                      },
    { .location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Mesh::Vertex3D, normal) },
    { .location = 2, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Mesh::Vertex3D, color) }
};
