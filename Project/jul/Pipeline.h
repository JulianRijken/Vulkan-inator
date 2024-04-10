#pragma once

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

#include "jul/Camera.h"
#include "jul/DescriptorPool.h"
#include "jul/Mesh.h"
#include "jul/Shader.h"

class Pipeline
{
public:
    Pipeline(const Shader& shader, Camera* camera, VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateInfo);

    ~Pipeline();

    void Draw(VkCommandBuffer commandBuffer, int imageIndex);
    void AddMesh(Mesh&& mesh);

private:
    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    void CreateDescriptorSetLayout();
    void CreateUniformbuffers(int maxFramesCount);

    void UpdateUniformBufferObject(int imageIndex, const VkExtent2D& swapChainExtent);

    VkPipeline m_Pipeline{};
    VkPipelineLayout m_PipelineLayout{};

    VkDescriptorSetLayout m_DescriptorSetlayout{};

    std::vector<std::unique_ptr<Buffer>> m_UniformBuffers{};
    std::unique_ptr<DescriptorPool> m_DescriptorPoolUPtr;

    VkRenderPass m_RenderPass;

    Camera* m_Camera = nullptr;

    std::vector<Mesh> m_Meshes{};
};
