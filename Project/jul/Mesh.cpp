#include "Mesh.h"
#include <stdexcept>


VkVertexInputBindingDescription Mesh::Vertex::GetBindingDescriptor()
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> Mesh::Vertex::GetAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}


// TODO: where should I put this?
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return -1;
}

Mesh::Mesh(VkDevice device, std::vector<Vertex> verts, VkPhysicalDevice physicalDevice) :
    m_Device{ device },
    m_NumVerts{ static_cast<uint32_t>(verts.size()) }
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(Vertex) * verts.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_VertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }


    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, m_VertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        physicalDevice);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }
    vkBindBufferMemory(device, m_VertexBuffer, m_VertexBufferMemory, 0);


    // time to fill the buffer
    {
        void* data;
        vkMapMemory(device, m_VertexBufferMemory, 0, bufferInfo.size, 0, &data);

        memcpy(data, verts.data(), (size_t) bufferInfo.size);

        vkUnmapMemory(device, m_VertexBufferMemory);
    }

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
