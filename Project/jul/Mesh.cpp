#include "Mesh.h"
#include "vulkanbase/VulkanUtil.h"


const VkVertexInputBindingDescription Mesh::Vertex::BINDING_DESCRIPTION
{
    .binding = 0,
    .stride = sizeof(Vertex),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};


const std::array<VkVertexInputAttributeDescription, 2> Mesh::Vertex::ATTRIBUTE_DESCRIPTIONS
{
    VkVertexInputAttributeDescription
    {
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex, pos),
    },
    {
        .location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, color)
    }
};




Mesh::Mesh(VkDevice device, std::vector<Vertex> vertices, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue) :
    m_Device{ device },
    m_NumVerts{ static_cast<uint32_t>(vertices.size()) }
{
    VkDeviceSize bufferSize{sizeof(Vertex) * vertices.size()};


    // Create staging buffer
    std::tie(m_StagingBuffer, m_StagingBufferMemory) = VkUtils::CreateBuffer(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        device,
        physicalDevice);


    // Fill staging buffer
    void* data;
    vkMapMemory(device, m_StagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, m_StagingBufferMemory);


    // Create Vertex buffer
    std::tie(m_VertexBuffer, m_VertexBufferMemory) = VkUtils::CreateBuffer(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        device,
        physicalDevice);


    VkUtils::CopyBuffer(m_StagingBuffer,m_VertexBuffer,bufferSize,device,graphicsQueue);
}

Mesh::~Mesh()
{
    vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
    vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);
}

void Mesh::Draw(VkCommandBuffer commandBuf) const
{
    VkBuffer vertexBuffers[] = {m_VertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuf, 0, 1, vertexBuffers, offsets);

    vkCmdDraw(commandBuf, static_cast<uint32_t>(m_NumVerts), 1, 0, 0);
}
