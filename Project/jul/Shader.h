#pragma once

#include <filesystem>
#include <vulkan/vulkan_core.h>
#include <array>

using path = std::filesystem::path;

class Shader final
{
public:

	Shader() = default;

	void Initialize(const path& vertexPath, const path& fragmentPath, VkDevice device);
	void DestroyShaderModules(VkDevice device) const;

	Shader(Shader&&) = delete;
	Shader(const Shader&) = delete;
	Shader& operator=(Shader&&) = delete;
	Shader& operator=(const Shader&) = delete;


	[[nodiscard]] const VkPipelineShaderStageCreateInfo& GetVertexInfo() const  { return m_VertexInfo; }
	[[nodiscard]] const VkPipelineShaderStageCreateInfo& GetFragmentInfo() const { return m_FragmentInfo; }


    [[nodiscard]] VkPipelineVertexInputStateCreateInfo   CreateVertexInputStateInfo();
    [[nodiscard]] VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();

private:

	[[nodiscard]] VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice device);

    VkVertexInputBindingDescription bindingDescription;
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions;

	VkPipelineShaderStageCreateInfo m_VertexInfo{};
	VkPipelineShaderStageCreateInfo m_FragmentInfo{};

};
