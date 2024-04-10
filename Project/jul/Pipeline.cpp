#include "Pipeline.h"

#include "SwapChain.h"
#include "vulkanbase/VulkanGlobals.h"

Pipeline::Pipeline(const Shader& shader, Camera* camera,
                   VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateInfo) :
    m_RenderPass(*&VulkanGlobals::GetRederPass()),
    m_Camera(camera)
{
    CreateDescriptorSetLayout();
    CreateUniformbuffers(VulkanGlobals::GetSwapChain().GetImageCount());


    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetlayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    
    if(vkCreatePipelineLayout(VulkanGlobals::GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create pipeline layout!");


    VkGraphicsPipelineCreateInfo pipelineInfo{};

    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;


    VkPipelineShaderStageCreateInfo shaderStages[] = { shader.GetVertexInfo(), shader.GetFragmentInfo() };

    auto inputAssemblyState{ Shader::CreateInputAssemblyStateInfo() };

    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &pipelineVertexInputStateInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;

    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = m_RenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    
    if(vkCreateGraphicsPipelines(VulkanGlobals::GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) !=
       VK_SUCCESS)
        throw std::runtime_error("failed to create graphics pipeline!");


    m_DescriptorPoolUPtr = std::make_unique<DescriptorPool>(VulkanGlobals::GetDevice(),
                                                            VulkanGlobals::GetSwapChain().GetImageCount(),
                                                            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                            m_DescriptorSetlayout,
                                                            m_UniformBuffers);
}

Pipeline::~Pipeline()
{
    vkDestroyPipeline(VulkanGlobals::GetDevice(), m_Pipeline, nullptr);
    vkDestroyPipelineLayout(VulkanGlobals::GetDevice(), m_PipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(VulkanGlobals::GetDevice(), m_DescriptorSetlayout, nullptr);
}

void Pipeline::Draw(VkCommandBuffer commandBuffer, int imageIndex)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(VulkanGlobals::GetSwapChain().GetExtent().width);
    viewport.height = static_cast<float>(VulkanGlobals::GetSwapChain().GetExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = VulkanGlobals::GetSwapChain().GetExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    UpdateUniformBufferObject(imageIndex, VulkanGlobals::GetSwapChain().GetExtent());

    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_PipelineLayout,
                            0,
                            1,
                            m_DescriptorPoolUPtr->GetDescriptorSet(imageIndex),
                            0,
                            nullptr);


    for(auto&& mesh : m_Meshes)
        mesh.Draw(commandBuffer);
}

void Pipeline::AddMesh(Mesh&& mesh) { m_Meshes.emplace_back(std::move(mesh)); }

void Pipeline::CreateDescriptorSetLayout()
{
    const VkDescriptorSetLayoutBinding uboLayoutBinding{

        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr
    };

    const VkDescriptorSetLayoutCreateInfo layoutInfo{

        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, .bindingCount = 1, .pBindings = &uboLayoutBinding
    };
    
    if(vkCreateDescriptorSetLayout(VulkanGlobals::GetDevice(), &layoutInfo, nullptr, &m_DescriptorSetlayout) != VK_SUCCESS)
        throw std::runtime_error("fialed to create descriptor set layout!");
}

void Pipeline::CreateUniformbuffers(int maxFramesCount)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_UniformBuffers.reserve(maxFramesCount);

    for(size_t i = 0; i < maxFramesCount; i++)
    {
        m_UniformBuffers.emplace_back(
            std::make_unique<Buffer>(bufferSize,
                                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

        m_UniformBuffers[i]->Map(bufferSize);
    }
}

void Pipeline::UpdateUniformBufferObject(int imageIndex, const VkExtent2D& swapChainExtent)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    const float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


    UniformBufferObject ubo{};

    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.view = m_Camera->GetViewMatrix();
    ubo.proj = m_Camera->GetProjectionMatrix();

    // Flip the Y as described in the vulcan tutorial
    ubo.proj[1][1] *= -1;

    m_UniformBuffers[imageIndex]->Upload(&ubo, sizeof(ubo));
}
