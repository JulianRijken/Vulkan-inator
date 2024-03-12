#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

class Mesh
{
public:
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		[[nodiscard]] static VkVertexInputBindingDescription GetBindingDescriptor();
		[[nodiscard]] static std::array<VkVertexInputAttributeDescription,2> GetAttributeDescriptions();
	};
};
