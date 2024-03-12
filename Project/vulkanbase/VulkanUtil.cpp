#include "VulkanUtil.h"
#include <fstream>
#include <jul/CommandBuffer.h>
#include <memory>
#include <string>

VkResult VkUtils::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VkUtils::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

std::vector<char> VkUtils::ReadFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}


VkUtils::QueueFamilyIndices VkUtils::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	VkUtils::QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) 
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    return -1;
}


std::tuple<VkBuffer, VkDeviceMemory> VkUtils::CreateBuffer(VkDeviceSize size,
                                                           VkBufferUsageFlags usage,
                                                           VkMemoryPropertyFlags properties,
                                                           VkDevice device,
                                                           VkPhysicalDevice physicalDevice)
{

    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_VertexBuffer) != VK_SUCCESS)
        throw std::runtime_error("failed to create vertex buffer!");



    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, m_VertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        memRequirements.memoryTypeBits,
        properties,
        physicalDevice);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate vertex buffer memory!");

    vkBindBufferMemory(device, m_VertexBuffer, m_VertexBufferMemory, 0);

    return {m_VertexBuffer,m_VertexBufferMemory};
}

void VkUtils::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDevice device, VkQueue graphicsQueue)
{
    auto commandBufferUPtr = std::make_unique<CommandBuffer>(device);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;

    commandBufferUPtr->BeginCommandBuffer();
    {
        vkCmdCopyBuffer(commandBufferUPtr->Get(),srcBuffer,dstBuffer,1,&copyRegion);
    }
    commandBufferUPtr->EndCommandBuffer();

    // Needed in order to pass ref
    VkCommandBuffer shittyCopyElision{commandBufferUPtr->Get()};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &shittyCopyElision;

    vkQueueSubmit(graphicsQueue , 1 , & submitInfo , VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);


}
