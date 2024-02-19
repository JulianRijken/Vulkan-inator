#include "ShaderModule.h"
#include "vulkanbase/VulkanBase.h"

ShaderModule::ShaderModule(const path& vertexPath, const path& fragmentPath, VkDevice device)
{
	//////////////////////////////
	/// Create vertex shader info
	//////////////////////////////
	const std::vector<char> vertShaderCode = readFile(vertexPath.string());
	const VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, device);

	m_VertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_VertexInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	m_VertexInfo.module = vertShaderModule;
	m_VertexInfo.pName = "main";


	//////////////////////////////
	/// Create fragment shader info
	//////////////////////////////
	const std::vector<char> fragShaderCode = readFile(fragmentPath.string());
	const VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, device);

	m_FragmentInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_FragmentInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	m_FragmentInfo.module = fragShaderModule;
	m_FragmentInfo.pName = "main";


}

ShaderModule::~ShaderModule()
{

}


VkShaderModule ShaderModule::createShaderModule(const std::vector<char>& code, VkDevice device)
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




