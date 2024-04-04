#include "Shader.h"
#include "vulkanbase/VulkanUtil.h"

Shader::Shader(const path& vertexPath, const path& fragmentPath, VkDevice device) :
    m_Device(device)
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

Shader::~Shader()
{
    if(m_VertexInfo.module != VK_NULL_HANDLE)
        vkDestroyShaderModule(m_Device, m_VertexInfo.module, nullptr);

    if(m_FragmentInfo.module != VK_NULL_HANDLE)
        vkDestroyShaderModule(m_Device, m_FragmentInfo.module, nullptr);
}

Shader::Shader(Shader&& other) noexcept :
    m_VertexInfo(other.m_VertexInfo),
    m_Device(other.m_Device),
    m_FragmentInfo(other.m_FragmentInfo)
{
    other.m_VertexInfo.module = VK_NULL_HANDLE;
    other.m_FragmentInfo.module = VK_NULL_HANDLE;
}

VkPipelineInputAssemblyStateCreateInfo Shader::CreateInputAssemblyStateInfo() const
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
