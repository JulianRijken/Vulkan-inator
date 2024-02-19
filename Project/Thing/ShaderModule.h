#pragma once

#include <filesystem>
#include <vulkan/vulkan_core.h>

using path = std::filesystem::path;

class ShaderModule
{
public:

	ShaderModule(const path& vertexPath, const path& fragmentPath, VkDevice device);
	~ShaderModule();

	[[nodiscard]] const VkPipelineShaderStageCreateInfo& getVertexInfo() const { return m_VertexInfo; }
	[[nodiscard]] const VkPipelineShaderStageCreateInfo& getFragmentInfo() const { return m_FragmentInfo; }

	[[nodiscard]] VkShaderModule createShaderModule(const std::vector<char>& code, VkDevice device);

private:
	VkPipelineShaderStageCreateInfo m_VertexInfo{};
	VkPipelineShaderStageCreateInfo m_FragmentInfo{};

};