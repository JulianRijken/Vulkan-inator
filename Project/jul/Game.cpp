#include "Game.h"

#include "jul/GameTime.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>


Game::Game()
{
    m_Pipline2D = std::make_unique<Pipeline>(Shader{ "shaders/shader2D.vert.spv", "shaders/shader2D.frag.spv" },
                                             Shader::CreateVertexInputStateInfo<Mesh::Vertex2D>(),
                                             sizeof(UniformBufferObject2D),
                                             VK_CULL_MODE_NONE);

    m_Pipline3D = std::make_unique<Pipeline>(Shader{ "shaders/shader3D.vert.spv", "shaders/shader3D.frag.spv" },
                                             Shader::CreateVertexInputStateInfo<Mesh::Vertex3D>(),
                                             sizeof(UniformBufferObject3D));


    {
        const std::vector<Mesh::Vertex2D> vertices = {
            {{ 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f }},
            { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }}
        };

        const std::vector<uint32_t> indeces = { 0, 1, 2 };


        AddMesh(Mesh{
            indeces,
            Mesh::VertexData{.data = (void*)vertices.data(),
                             .vertexCount = static_cast<uint32_t>(vertices.size()),
                             .typeSize = sizeof(Mesh::Vertex2D)}
        });
    }

    {

        const std::vector<Mesh::Vertex3D> vertices{
  // front
            { { -0.5f, -0.5f, 0.5f }, {}, { 1, 0, 0 }}, // 0
            {  { 0.5f, -0.5f, 0.5f }, {}, { 0, 1, 0 }}, // 1
            {   { 0.5f, 0.5f, 0.5f }, {}, { 1, 1, 0 }}, // 2
            {  { -0.5f, 0.5f, 0.5f }, {}, { 0, 0, 1 }}, // 3

  // back
            {{ -0.5f, -0.5f, -0.5f }, {}, { 1, 0, 1 }}, // 4
            { { 0.5f, -0.5f, -0.5f }, {}, { 0, 1, 1 }}, // 5
            {  { 0.5f, 0.5f, -0.5f }, {}, { 1, 1, 1 }}, // 6
            { { -0.5f, 0.5f, -0.5f }, {}, { 1, 0, 1 }}  // 7
        };

        // clang-format off
            const std::vector<uint32_t> indeces{
                0, 1, 2, 2, 3, 0, // front
                1, 5, 6, 6, 2, 1, // right
                5, 4, 7, 7, 6, 5, // back
                4, 0, 3, 3, 7, 4, // left
                3, 2, 6, 6, 7, 3, // top
                4, 5, 1, 1, 0, 4  // bottom
            };
        // clang-format on


            AddMesh(Mesh{
                indeces,
                Mesh::VertexData{.data = (void*)vertices.data(),
                                 .vertexCount = static_cast<uint32_t>(vertices.size()),
                                 .typeSize = sizeof(Mesh::Vertex3D)}
            });
    }
}

void Game::Update() { m_Camera.Update(); }

void Game::Draw(VkCommandBuffer commandBuffer, int imageIndex)
{
    // 2D Meshesh
    UniformBufferObject2D ubo2D{};
    {
        ubo2D.model = glm::rotate(
            glm::mat4(1.0f), jul::GameTime::GetElapsedTimeF() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo2D.model[3][0] = 0.5f;
        ubo2D.proj = m_Camera.GetOrthoProjectionMatrix();
    }

    m_Pipline2D->Bind(commandBuffer, imageIndex);
    m_Pipline2D->UpdateUBO(imageIndex, &ubo2D, sizeof(ubo2D));
    m_Meshes[0].Draw(commandBuffer);


    // 3D Meshesh
    UniformBufferObject3D ubo3D{};
    {
        ubo3D.model = glm::rotate(
            glm::mat4(1.0f), jul::GameTime::GetElapsedTimeF() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo3D.model[3][0] = 1;
        ubo3D.view = m_Camera.GetViewMatrix();
        ubo3D.proj = m_Camera.GetProjectionMatrix();
        ubo3D.proj[1][1] *= -1;
    }

    m_Pipline3D->Bind(commandBuffer, imageIndex);
    m_Pipline3D->UpdateUBO(imageIndex, &ubo3D, sizeof(ubo3D));
    m_Meshes[1].Draw(commandBuffer);
}
