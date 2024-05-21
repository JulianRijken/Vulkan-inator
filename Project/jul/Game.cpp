#include "Game.h"

#include <iostream>

#include "jul/GameTime.h"
#include "jul/MathExtensions.h"
#include "jul/SwapChain.h"
#include "jul/Texture.h"

#define GLM_FORCE_RADIANS
#include <tiny_obj_loader.h>
#include <vulkanbase/VulkanGlobals.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>


Game::Game()
{
    m_Textures["Grass"] = std::make_unique<Texture>("resources/Diorama/T_Grass_Color.png");
    m_Textures["Car"] = std::make_unique<Texture>("resources/Diorama/T_FordGT40_Color.png");
    m_Textures["Konker"] = std::make_unique<Texture>("resources/Diorama/T_Konker_Color.png");
    m_Textures["Clothing"] = std::make_unique<Texture>("resources/Diorama/T_Clothing_Color.png");


    m_Textures["subaru_Outside_BaseColor"] = std::make_unique<Texture>("resources/Car/subaru_Outside_BaseColor.png");
    m_Textures["subaru_Outside_Metallic"] = std::make_unique<Texture>("resources/Car/subaru_Outside_Metallic.png");
    m_Textures["subaru_Outside_Normal"] = std::make_unique<Texture>("resources/Car/subaru_Outside_Normal.png");
    m_Textures["subaru_Outside_Roughness"] = std::make_unique<Texture>("resources/Car/subaru_Outside_Roughness.png");

    // m_Textures["JAKUB"] = std::make_unique<Texture>("resources/Testing/Jakub.png");
    // m_Textures["SWORD"] = std::make_unique<Texture>("resources/Testing/Sword.jpg");


    m_Materials["PBR"] =
        std::make_unique<Material>(std::vector<const Texture*>{ m_Textures["subaru_Outside_BaseColor"].get(),
                                                                m_Textures["subaru_Outside_Metallic"].get(),
                                                                m_Textures["subaru_Outside_Normal"].get(),
                                                                m_Textures["subaru_Outside_Roughness"].get() });

    // m_Pipline2D = std::make_unique<Pipeline>(Shader{ "shaders/shader2D.vert.spv", "shaders/shader2D.frag.spv" },
    //                                          Shader::CreateVertexInputStateInfo<Mesh::Vertex2D>(),
    //                                          sizeof(UniformBufferObject2D),
    //                                          sizeof(MeshPushConstants),
    //                                          std::nullopt,
    //                                          VK_CULL_MODE_NONE,
    //                                          VK_FALSE,
    //                                          VK_FALSE);

    m_Pipline3D = std::make_unique<Pipeline>(Shader{ "shaders/shader3D.vert.spv", "shaders/shader3D.frag.spv" },
                                             Shader::CreateVertexInputStateInfo<Mesh::Vertex3D>(),
                                             sizeof(UniformBufferObject3D),
                                             sizeof(MeshPushConstants),
                                             Material::GetMaterialSetLayout(),
                                             VK_CULL_MODE_NONE);


    // const std::vector<Mesh::Vertex2D> triangleVertices = {
    //     {{ 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f }},
    //     { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }},
    //     {{ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }}
    // };

    // const std::vector<uint32_t> triangleIndeces = { 0, 1, 2 };


    // AddMesh(Mesh{
    //     triangleIndeces,
    //     Mesh::VertexData{.data = (void*)triangleVertices.data(),
    //                      .vertexCount = static_cast<uint32_t>(triangleVertices.size()),
    //                      .typeSize = sizeof(Mesh::Vertex2D)}
    // });


    // const std::vector<Mesh::Vertex2D> squareVertices = {
    //     {{ -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }},
    //     { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }},
    //     {  { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }},
    //     { { -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }}
    // };

    // const std::vector<uint32_t> squareIndices = { 0, 1, 2, 0, 2, 3 };

    // AddMesh(Mesh{
    //     squareIndices,
    //     Mesh::VertexData{.data = (void*)squareVertices.data(),
    //                      .vertexCount = static_cast<uint32_t>(squareVertices.size()),
    //                      .typeSize = sizeof(Mesh::Vertex2D)}
    // });

    // AddMesh(GenerateCircle({ 0, 0 }, { 0.4f, 0.6f }));


    // AddMesh(LoadMesh("resources/Diorama/DioramaGP.obj"));
    AddMesh(LoadMesh("resources/Car/Subaru.obj"));
    // AddMesh(LoadMesh("resources/Airplane/Airplane.obj"));
    // AddMesh(LoadMesh("resources/Terrain/Terrain.obj"));

    // AddMesh(LoadMesh("resources/Testing/Jakub.obj"));
    // AddMesh(LoadMesh("resources/Testing/Sword.obj"));
}

Game::~Game() = default;

void Game::Update() { m_Camera.Update(); }

void Game::Draw(VkCommandBuffer commandBuffer, int imageIndex)
{
    // MeshPushConstants meshPushConstant2D2{};
    // {
    //     meshPushConstant2D2.model = glm::rotate(
    //         glm::mat4(1.0f), -jul::GameTime::GetElapsedTimeF() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //     meshPushConstant2D2.model[3][0] = -1.0f;
    // }
    // m_Pipline2D->UpdatePushConstant(commandBuffer, &meshPushConstant2D2, sizeof(meshPushConstant2D2));
    // m_Meshes[1].Draw(commandBuffer);


    // 3D Meshesh
    UniformBufferObject3D ubo3D{};
    {
        auto projectionMatrix = m_Camera.GetProjectionMatrix();
        projectionMatrix[1][1] *= -1;

        ubo3D.viewProjection = projectionMatrix * m_Camera.GetViewMatrix();
        ubo3D.viewPosition = glm::vec4(m_Camera.GetPosition(), 1.0f);
    }


    m_Pipline3D->Bind(commandBuffer, imageIndex);
    m_Pipline3D->UpdateUBO(imageIndex, &ubo3D, sizeof(ubo3D));

    MeshPushConstants meshPushConstantDiorama{};
    {
        meshPushConstantDiorama.model =
            glm::rotate(
                glm::mat4(1.0f), jul::GameTime::GetElapsedTimeF() * glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), { 0.1f, 0.1f, 0.1f });
    }

    m_Pipline3D->UpdateMaterial(commandBuffer, *m_Materials["PBR"]);
    m_Pipline3D->UpdatePushConstant(commandBuffer, &meshPushConstantDiorama, sizeof(meshPushConstantDiorama));
    m_Meshes[0].Draw(commandBuffer);


    // const float planePosition = jul::math::ClampLoop(jul::GameTime::GetElapsedTimeF() * 50.0f, -100.0f, 100.0f);
    // MeshPushConstants meshPushConstantPlane{
    //     .model = glm::translate(glm::mat4(1.0f), { 0, 25, planePosition }) *
    //              glm::scale(glm::mat4(1.0f), { 0.01f, 0.01f, 0.01f }) *
    //              glm::rotate(glm::mat4(1.0f), jul::GameTime::GetElapsedTimeF(), { 0, 0, 1 })
    // };


    // m_Pipline3D->UpdateMaterial(commandBuffer, *m_Materials["PBR"]);
    // m_Pipline3D->UpdatePushConstant(commandBuffer, &meshPushConstantPlane, sizeof(meshPushConstantPlane));
    // m_Meshes[3].Draw(commandBuffer);


    // MeshPushConstants TerrainMesh{ .model = glm::translate(glm::mat4(1.0f), { -50, 0, 50 }) *
    //                                         glm::scale(glm::mat4(1.0f), { 0.1f, 0.1f, 0.1f }) *
    //                                         glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), { 0, 1, 0 }) };

    // m_Pipline3D->UpdatePushConstant(commandBuffer, &TerrainMesh, sizeof(TerrainMesh));
    // m_Meshes[3].Draw(commandBuffer);
}

void Game::OnResize() { m_Camera.SetAspect(VulkanGlobals::GetSwapChain().GetAspect()); }

Mesh Game::LoadMesh(const std::string& meshPath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    if(not tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshPath.c_str()))
        std::cerr << "Loading OBJ Failed: \n";


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
            Mesh::Vertex3D vertex{
                .position = { attrib.vertices[3 * index.vertex_index + 0],
                             attrib.vertices[3 * index.vertex_index + 1],
                             attrib.vertices[3 * index.vertex_index + 2] },
                .normal = { attrib.normals[3 * index.normal_index + 0],
                             attrib.normals[3 * index.normal_index + 1],
                             attrib.normals[3 * index.normal_index + 2] },
                .tangent = { 
                             
                             },
                .uv = { attrib.texcoords[2 * index.texcoord_index + 0],
                             1.0f - attrib.texcoords[2 * index.texcoord_index + 1] }
            };


            vertex.tangent = glm::cross(vertex.position, vertex.normal);

            vertex.tangent = glm::normalize(vertex.tangent);
            vertex.normal = glm::normalize(vertex.normal);

            if(not uniqueVertices.contains(vertex))
            {
                uniqueVertices[vertex] = vertices.size();
                vertices.push_back(vertex);
            }

            indeces.push_back(uniqueVertices[vertex]);
        }
    }

    std::cout << "Loaded: " << meshPath << " Successfully :)\n";

    return Mesh{
        indeces,
        Mesh::VertexData{.data = (void*)vertices.data(),
                         .vertexCount = static_cast<uint32_t>(vertices.size()),
                         .typeSize = sizeof(Mesh::Vertex3D)}
    };
}

Mesh Game::GenerateCircle(glm::vec2 center, glm::vec2 size, uint32_t segmentCount)
{
    std::vector<Mesh::Vertex2D> circleVertices{};
    std::vector<uint32_t> circleIndices{};

    const glm::vec3 innerColor{ 0, 0, 1 };
    const glm::vec3 outerColor{ 0, 1, 0 };

    // Cetner
    circleVertices.push_back({ center, innerColor });

    for(uint32_t segment = 0; segment < segmentCount; segment++)
    {
        const float angle = static_cast<float>(segment) / static_cast<float>(segmentCount - 1) * (glm::pi<float>() * 2);
        const glm::vec2 offset = { std::cos(angle) * size.x, std::sin(angle) * size.y };

        circleVertices.push_back({ .position = center + offset, .color = outerColor * angle / (glm::pi<float>() * 2) });

        circleIndices.push_back(0);
        circleIndices.push_back(segment + 1);
        circleIndices.push_back(segment < segmentCount - 1 ? segment + 2 : 1);  // Flip
    }


    // Add the circle to the mesh
    return {
        circleIndices,
        Mesh::VertexData{.data = (void*)circleVertices.data(),
                         .vertexCount = static_cast<uint32_t>(circleVertices.size()),
                         .typeSize = sizeof(Mesh::Vertex2D)}
    };
}
