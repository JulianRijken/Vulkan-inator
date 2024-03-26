#include "Mesh.h"
#include "vulkanbase/VulkanUtil.h"


Mesh::Mesh(VkDevice device,const VertexData& vertexData, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue) :
    m_Device{ device },
    m_NumVerts{ vertexData.vertexCount}
{
    VkDeviceSize const BUFFER_SIZE{vertexData.typeSize * vertexData.vertexCount};


    // Create staging buffer
    std::tie(m_StagingBuffer, m_StagingBufferMemory) = VkUtils::CreateBuffer(
        BUFFER_SIZE,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        device,
        physicalDevice);


    // Fill staging buffer
    void* data = nullptr;
    vkMapMemory(device, m_StagingBufferMemory, 0, BUFFER_SIZE, 0, &data);
    memcpy(data, vertexData.data, (size_t)BUFFER_SIZE);
    vkUnmapMemory(device, m_StagingBufferMemory);


    // Create Vertex buffer
    std::tie(m_VertexBuffer, m_VertexBufferMemory) = VkUtils::CreateBuffer(
        BUFFER_SIZE,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        device,
        physicalDevice);


    VkUtils::CopyBuffer(m_StagingBuffer,m_VertexBuffer,BUFFER_SIZE,device,graphicsQueue);
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

const VkVertexInputBindingDescription Mesh::Vertex2D::BINDING_DESCRIPTION
{

    .binding = 0,
    .stride = sizeof(Mesh::Vertex2D),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX

};

const std::array<VkVertexInputAttributeDescription,2> Mesh::Vertex2D::ATTRIBUTE_DESCRIPTIONS
    {

        VkVertexInputAttributeDescription
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex2D, pos),
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex2D, color)
        }

    };




const VkVertexInputBindingDescription Mesh::Vertex3D::BINDING_DESCRIPTION
    {

        .binding = 0,
        .stride = sizeof(Mesh::Vertex3D),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX

    };

const std::array<VkVertexInputAttributeDescription,3> Mesh::Vertex3D::ATTRIBUTE_DESCRIPTIONS
    {

        VkVertexInputAttributeDescription
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Mesh::Vertex3D, pos),
        },
        {
            .location = 1,
            .bind ing = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Mesh::Vertex3D, color)
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Mesh::Vertex3D, color)
        }

    };



