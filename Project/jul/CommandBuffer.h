#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>

class CommandBuffer final
{
public:
    CommandBuffer(VkDevice device,uint32_t familyIndex = 0);
	~CommandBuffer();

    void BeginBuffer();
    void EndBuffer();

    operator VkCommandBuffer() const { return m_CommandBuffer; }

private:
    static inline VkCommandPool s_CommandPool{nullptr};
    static inline int s_AliveBuffers{};

	VkCommandBuffer m_CommandBuffer{};
	VkDevice m_Device;
};
