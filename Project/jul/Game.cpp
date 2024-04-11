#include "Game.h"

#include <iostream>

#include "jul/GameTime.h"
#include "jul/MathExtensions.h"

#define GLM_FORCE_RADIANS
#include <tiny_obj_loader.h>

#include <glm/gtc/matrix_transform.hpp>

Game::Game()
{
    m_Pipline2D = std::make_unique<Pipeline>(Shader{ "shaders/shader2D.vert.spv", "shaders/shader2D.frag.spv" },
                                             Shader::CreateVertexInputStateInfo<Mesh::Vertex2D>(),
                                             sizeof(UniformBufferObject2D),
                                             sizeof(MeshPushConstants),
                                             VK_CULL_MODE_NONE);

    m_Pipline3D = std::make_unique<Pipeline>(Shader{ "shaders/shader3D.vert.spv", "shaders/shader3D.frag.spv" },
                                             Shader::CreateVertexInputStateInfo<Mesh::Vertex3D>(),
                                             sizeof(UniformBufferObject3D),
                                             sizeof(MeshPushConstants));


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


        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warn;
        std::string err;

        // tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "resources/Airplane/Airplane.obj");
        tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "resources/Diorama/DioramaGP.obj");

        if(not warn.empty())
            std::cerr << "Loading OBJ Failed: " << warn << '\n';
        if(not err.empty())
            std::cerr << "Loading OBJ Failed: " << err << '\n';


        std::vector<Mesh::Vertex3D> vertices{};
        std::vector<uint32_t> indeces{};


        std::unordered_map<Mesh::Vertex3D, uint32_t> uniqueVertices{};

        for(auto&& shape : shapes)
        {
            for(auto&& index : shape.mesh.indices)
            {
                const Mesh::Vertex3D vertex{
                    .pos = {attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]},
                    .normal = { attrib.normals[3 * index.vertex_index + 0],
                            attrib.normals[3 * index.vertex_index + 1],
                            attrib.normals[3 * index.vertex_index + 2] },
                    .color = {            jul::math::RandomValue<float>(),
                            jul::math::RandomValue<float>(),
                            jul::math::RandomValue<float>()            }
                };

                if(not uniqueVertices.contains(vertex))
                {
                    uniqueVertices[vertex] = vertices.size();
                    vertices.push_back(vertex);
                }

                indeces.push_back(uniqueVertices[vertex]);
            }
        }

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
    // // 2D Meshesh
    // UniformBufferObject2D ubo2D{};
    // {
    //     ubo2D.proj = m_Camera.GetOrthoProjectionMatrix();
    // }

    // MeshPushConstants meshPushConstant2D{};
    // {
    //     meshPushConstant2D.model = glm::rotate(
    //         glm::mat4(1.0f), jul::GameTime::GetElapsedTimeF() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //     meshPushConstant2D.model[3][0] = 0.5f;
    // }


    // m_Pipline2D->Bind(commandBuffer, imageIndex);
    // m_Pipline2D->UpdateUBO(imageIndex, &ubo2D, sizeof(ubo2D));
    // m_Pipline2D->UpdatePushConstant(commandBuffer, &meshPushConstant2D, sizeof(meshPushConstant2D));
    // m_Meshes[0].Draw(commandBuffer);


    // 3D Meshesh
    UniformBufferObject3D ubo3D{};
    {
        ubo3D.view = m_Camera.GetViewMatrix();
        ubo3D.proj = m_Camera.GetProjectionMatrix();
        ubo3D.proj[1][1] *= -1;
    }

    MeshPushConstants meshPushConstant3D{};
    {
        meshPushConstant3D.model = glm::rotate(
            glm::mat4(1.0f), jul::GameTime::GetElapsedTimeF() * glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    m_Pipline3D->Bind(commandBuffer, imageIndex);
    m_Pipline3D->UpdateUBO(imageIndex, &ubo3D, sizeof(ubo3D));
    m_Pipline3D->UpdatePushConstant(commandBuffer, &meshPushConstant3D, sizeof(meshPushConstant3D));
    m_Meshes[1].Draw(commandBuffer);
}
