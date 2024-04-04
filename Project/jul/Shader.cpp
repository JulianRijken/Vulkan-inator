#include "Shader.h"
#include "vulkanbase/VulkanUtil.h"


Shader::Shader(const path& vertexPath, const path& fragmentPath, VkDevice device)
{
    //////////////////////////////
    /// Create vertex shader info
    //////////////////////////////
    const std::vector<char> VERT_SHADER_CODE = VkUtils::ReadFile(vertexPath.string());
    const VkShaderModule VERT_SHADER_MODULE = CreateShaderModule(VERT_SHADER_CODE, device);

    m_VertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_VertexInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    m_VertexInfo.module = VERT_SHADER_MODULE;
    m_VertexInfo.pName = "main";


    //////////////////////////////
    /// Create fragment shader info
    //////////////////////////////
    const std::vector<char> FRAG_SHADER_CODE = VkUtils::ReadFile(fragmentPath.string());
    const VkShaderModule FRAG_SHADER_MODULE = CreateShaderModule(FRAG_SHADER_CODE, device);

    m_FragmentInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_FragmentInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    m_FragmentInfo.module = FRAG_SHADER_MODULE;
    m_FragmentInfo.pName = "main";
}

void Shader::DestroyShaderModules(VkDevice device) const
{
	vkDestroyShaderModule(device, m_VertexInfo.module, nullptr);
	vkDestroyShaderModule(device, m_FragmentInfo.module, nullptr);
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
