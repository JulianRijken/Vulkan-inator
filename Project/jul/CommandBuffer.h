#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>

class CommandBuffer final
{
public:
    CommandBuffer(VkDevice device,uint32_t familyIndex = 0);
	~CommandBuffer();

	[[nodiscard]] VkCommandBuffer Get() const { return m_CommandBuffer; }

	void BeginCommandBuffer() const;
	void EndCommandBuffer() const;

private:
    static inline VkCommandPool s_CommandPool{nullptr};
    static inline int s_AliveBuffers{};

	VkCommandBuffer m_CommandBuffer{};
	VkDevice m_Device;
};
