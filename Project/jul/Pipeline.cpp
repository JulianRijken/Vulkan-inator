#include "Pipeline.h"

#include "SwapChain.h"
#include "vulkanbase/VulkanGlobals.h"

Pipeline::Pipeline(const Shader& shader, VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateInfo,
                   VkDeviceSize uboSize, uint32_t pushConstantSize, VkCullModeFlagBits cullMode,
                   VkBool32 depthTestEnable, VkBool32 depthWriteEnable) :
    m_RenderPass(*&VulkanGlobals::GetRederPass())
{
    CreateDescriptorSetLayout();
    CreateUniformbuffers(VulkanGlobals::GetSwapChain().GetImageCount(), uboSize);


    VkPipelineViewportStateCreateInfo viewportState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = static_cast<VkCullModeFlags>(cullMode),
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampling{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
    };

    VkPipelineDepthStencilStateCreateInfo depthStencil{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = depthTestEnable,
        .depthWriteEnable = depthWriteEnable,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0.f,
        .maxDepthBounds = 1.f,
    };


    VkPipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = VK_FALSE,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlending{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
    };


    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data(),
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &m_DescriptorSetlayout,
        .pushConstantRangeCount = 0,
    };

    const VkPushConstantRange pushConstant{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = pushConstantSize,
    };

    if(pushConstantSize > 0)
    {
        pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
    }

    if(vkCreatePipelineLayout(VulkanGlobals::GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) !=
       VK_SUCCESS)
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
    pipelineInfo.pDepthStencilState = &depthStencil;

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

void Pipeline::Bind(VkCommandBuffer commandBuffer, int imageIndex)
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

    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_PipelineLayout,
                            0,
                            1,
                            m_DescriptorPoolUPtr->GetDescriptorSet(imageIndex),
                            0,
                            nullptr);
}


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

void Pipeline::CreateUniformbuffers(int maxFramesCount, VkDeviceSize uboBufferSize)
{
    m_UniformBuffers.reserve(maxFramesCount);

    for(size_t i = 0; i < maxFramesCount; i++)
    {
        m_UniformBuffers.emplace_back(
            std::make_unique<Buffer>(uboBufferSize,
                                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

        m_UniformBuffers[i]->Map(uboBufferSize);
    }
}

void Pipeline::UpdateUBO(int imageIndex, void* uboData, VkDeviceSize uboSize)
{
    m_UniformBuffers[imageIndex]->Upload(uboData, uboSize);
}

void Pipeline::UpdatePushConstant(VkCommandBuffer commandBuffer, void* pushConstants, uint32_t pushConstantSize)
{
    vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, pushConstantSize, pushConstants);
}
