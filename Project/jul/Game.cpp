#include "Game.h"

Game::Game()
{
    m_Pipline2D = std::make_unique<Pipeline>(Shader{ "shaders/shader2D.vert.spv", "shaders/shader2D.frag.spv" },
                                             &m_Camera,
                                             Shader::CreateVertexInputStateInfo<Mesh::Vertex2D>());

    m_Pipline3D = std::make_unique<Pipeline>(Shader{ "shaders/shader3D.vert.spv", "shaders/shader3D.frag.spv" },
                                             &m_Camera,
                                             Shader::CreateVertexInputStateInfo<Mesh::Vertex3D>());


    {
        const std::vector<Mesh::Vertex2D> vertices = {
            {{ 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f }},
            { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }}
        };

        const std::vector<uint32_t> indeces = { 0, 1, 2 };


        m_Pipline2D->AddMesh(Mesh{
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


        m_Pipline3D->AddMesh(Mesh{
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
    m_Pipline2D->Draw(commandBuffer, imageIndex);
    m_Pipline3D->Draw(commandBuffer, imageIndex);
}
