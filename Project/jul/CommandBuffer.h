#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>

class CommandBuffer final
{
public:
	CommandBuffer(VkDevice device,uint32_t familyIndex);
	~CommandBuffer();

	[[nodiscard]] VkCommandBuffer Get() const { return m_CommandBuffer; }

	void BeginCommandBuffer() const;
	void EndCommandBuffer() const;

private:
	VkCommandPool m_CommandPool{};
	VkCommandBuffer m_CommandBuffer{};
	VkDevice m_Device;
};
