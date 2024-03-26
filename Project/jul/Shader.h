#pragma once

#include "Mesh.h"

#include <filesystem>
#include <vulkan/vulkan_core.h>

using path = std::filesystem::path;

class Shader final
{
public:

    Shader(const path& vertexPath, const path& fragmentPath, VkDevice device);

	void DestroyShaderModules(VkDevice device) const;

	Shader(Shader&&) = delete;
	Shader(const Shader&) = delete;
	Shader& operator=(Shader&&) = delete;
	Shader& operator=(const Shader&) = delete;


	[[nodiscard]] const VkPipelineShaderStageCreateInfo& GetVertexInfo() const  { return m_VertexInfo; }
	[[nodiscard]] const VkPipelineShaderStageCreateInfo& GetFragmentInfo() const { return m_FragmentInfo; }


    template<typename Vertex>
    [[nodiscard]] VkPipelineVertexInputStateCreateInfo   CreateVertexInputStateInfo()
    {
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &Vertex::BINDING_DESCRIPTION;

        vertexInputInfo.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(Vertex::ATTRIBUTE_DESCRIPTIONS.size());
        vertexInputInfo.pVertexAttributeDescriptions = Vertex::ATTRIBUTE_DESCRIPTIONS.data();

        return vertexInputInfo;
    }

    [[nodiscard]] VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();

private:

	[[nodiscard]] VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice device);

	VkPipelineShaderStageCreateInfo m_VertexInfo{};
	VkPipelineShaderStageCreateInfo m_FragmentInfo{};
};
