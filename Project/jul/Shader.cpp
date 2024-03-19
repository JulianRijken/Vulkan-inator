#include "Shader.h"

#include <array>

#include "Mesh.h"
#include "vulkanbase/VulkanUtil.h"


Shader::Shader(const path& vertexPath, const path& fragmentPath, VkDevice device)
{
    //////////////////////////////
    /// Create vertex shader info
    //////////////////////////////
    const std::vector<char> vertShaderCode = VkUtils::ReadFile(vertexPath.string());
    const VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode, device);

    m_VertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_VertexInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    m_VertexInfo.module = vertShaderModule;
    m_VertexInfo.pName = "main";


    //////////////////////////////
    /// Create fragment shader info
    //////////////////////////////
    const std::vector<char> fragShaderCode = VkUtils::ReadFile(fragmentPath.string());
    const VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode, device);

    m_FragmentInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_FragmentInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    m_FragmentInfo.module = fragShaderModule;
    m_FragmentInfo.pName = "main";
}

void Shader::DestroyShaderModules(VkDevice device) const
{
	vkDestroyShaderModule(device, m_VertexInfo.module, nullptr);
	vkDestroyShaderModule(device, m_FragmentInfo.module, nullptr);
}


VkPipelineVertexInputStateCreateInfo Shader::CreateVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &Mesh::Vertex::BINDING_DESCRIPTION;

    vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(Mesh::Vertex::ATTRIBUTE_DESCRIPTIONS.size());
    vertexInputInfo.pVertexAttributeDescriptions = Mesh::Vertex::ATTRIBUTE_DESCRIPTIONS.data();

    return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo Shader::CreateInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

VkShaderModule Shader::CreateShaderModule(const std::vector<char>& code, VkDevice device)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("failed to create shader module!");

	return shaderModule;
}
