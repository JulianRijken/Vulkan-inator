#include <set>

#include "vulkanbase/VulkanBase.h"
#include "vulkanbase/VulkanGlobals.h"
#include "vulkanbase/VulkanUtil.h"

void VulkanBase::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

    if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan support!");


	std::vector<VkPhysicalDevice> devices{ deviceCount };
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

    if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

    // TODO: Make sure it picks the dedicated wamm... gpu I mean :)
    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            m_PhysicalDevice = device;
			break;
		}
	}

    if (m_PhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("failed to find a suitable GPU!");

    VulkanGlobals::s_PhysicalDevice = m_PhysicalDevice;
}

bool VulkanBase::IsDeviceSuitable(VkPhysicalDevice device)
{
    VkUtils::QueueFamilyIndices indices = VkUtils::FindQueueFamilies(device,m_Surface);
    return indices.IsComplete() && CheckDeviceExtensionSupport(device);
}

void VulkanBase::CreateLogicalDevice()
{
    VkUtils::QueueFamilyIndices queueFamilyIndices = VkUtils::FindQueueFamilies(m_PhysicalDevice,m_Surface);
    std::set<uint32_t> uniqueQueueFamilies = { queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value() };


    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    {
        queueCreateInfos.reserve(uniqueQueueFamilies.size());


        const float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
    }


	VkDeviceCreateInfo createInfo{};
    {
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        const VkPhysicalDeviceFeatures  deviceFeatures{};
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
        createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
            createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        // Create the actual device
        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
            throw std::runtime_error("failed to create logical device!");

        VulkanGlobals::s_Device = m_Device;
    }

    vkGetDeviceQueue(m_Device, queueFamilyIndices.graphicsFamily.value(), 0, &m_GraphicsQueue);
    VulkanGlobals::s_GraphicsQueue = m_GraphicsQueue;

    vkGetDeviceQueue(m_Device, queueFamilyIndices.presentFamily.value(), 0, &m_PresentQueue);
}
