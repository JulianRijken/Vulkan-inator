#include "Game.h"

#include <iostream>

#include "jul/GameTime.h"
#include "jul/Input.h"
#include "jul/MathExtensions.h"
#include "jul/SwapChain.h"
#include "jul/Texture.h"

#define GLM_FORCE_RADIANS
#include <tiny_obj_loader.h>
#include <vulkanbase/VulkanGlobals.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/orthonormalize.hpp>

Game::Game()
{
    m_Textures["Grass"] = std::make_unique<Texture>("resources/Diorama/T_Grass_Color.png");
    m_Textures["Car"] = std::make_unique<Texture>("resources/Diorama/T_FordGT40_Color.png");
    m_Textures["Konker"] = std::make_unique<Texture>("resources/Diorama/T_Konker_Color.png");
    m_Textures["Clothing"] = std::make_unique<Texture>("resources/Diorama/T_Clothing_Color.png");

    m_Textures["defaultBlack"] = std::make_unique<Texture>("resources/Default/defaultBlack.png");
    m_Textures["defaultNormal"] = std::make_unique<Texture>("resources/Default/defaultNormal.png");
    m_Textures["defaultWhite"] = std::make_unique<Texture>("resources/Default/defaultWhite.png");
    m_Textures["uv_grid"] = std::make_unique<Texture>("resources/Default/uv_grid.png");
    m_Textures["uv_grid_2"] = std::make_unique<Texture>("resources/Default/uv_grid_2.png");
    m_Textures["uv_grid_3"] = std::make_unique<Texture>("resources/Default/uv_grid_3.png");

    m_Textures["subaru_Outside_BaseColor"] = std::make_unique<Texture>("resources/Car/subaru_Outside_BaseColor.png");
    m_Textures["subaru_Outside_Normal"] = std::make_unique<Texture>("resources/Car/subaru_Outside_Normal.png");
    m_Textures["subaru_Outside_Metallic"] = std::make_unique<Texture>("resources/Car/subaru_Outside_Metallic.png");
    m_Textures["subaru_Outside_Roughness"] = std::make_unique<Texture>("resources/Car/subaru_Outside_Roughness.png");

    m_Textures["fire_BaseColor"] = std::make_unique<Texture>("resources/FireHydrant/fire_hydrant_Base_Color.png");
    m_Textures["fire_Normal"] = std::make_unique<Texture>("resources/FireHydrant/fire_hydrant_Normal_OpenGL.png");
    m_Textures["fire_Metallic"] = std::make_unique<Texture>("resources/FireHydrant/fire_hydrant_Metallic.png");
    m_Textures["fire_Roughness"] = std::make_unique<Texture>("resources/FireHydrant/fire_hydrant_Roughness.png");


    m_Textures["robot_BaseColor"] = std::make_unique<Texture>("resources/Robot/robot_steampunk_color.png");
    m_Textures["robot_Normal"] = std::make_unique<Texture>("resources/Robot/robot_steampunk_nmap.png");
    m_Textures["robot_Metallic"] = std::make_unique<Texture>("resources/Robot/robot_steampunk_metalness.png");
    m_Textures["robot_Roughness"] = std::make_unique<Texture>("resources/Robot/robot_steampunk_rough.png");
    m_Textures["robot_ambientOcclusion"] = std::make_unique<Texture>("resources/Robot/robot_steampunk_ao.png");


    m_Textures["monkey_BaseColor"] = std::make_unique<Texture>("resources/Monkey/Suz_BaseColor.png");
    m_Textures["monkey_Normal"] = std::make_unique<Texture>("resources/Monkey/Suz_normal.png");
    m_Textures["monkey_Metallic"] = std::make_unique<Texture>("resources/Monkey/Suz_Metalness.png");
    m_Textures["monkey_Roughness"] = std::make_unique<Texture>("resources/Monkey/Suz_Roughness.png");

    m_Textures["water"] = std::make_unique<Texture>("resources/Water/WaterTexture.png");
    m_Textures["waterDis"] = std::make_unique<Texture>("resources/Water/water_ind.png");
    m_Materials["water"] = std::make_unique<Material>(std::vector<Texture*>{ m_Textures["water"].get(),
                                                                             m_Textures["waterDis"].get(),
                                                                             m_Textures["defaultBlack"].get(),
                                                                             m_Textures["defaultWhite"].get(),
                                                                             m_Textures["defaultWhite"].get() });


    m_Textures["skybox"] = std::make_unique<Texture>("resources/Skybox/Skybox_baseColor.png");

    m_Materials["skybox"] = std::make_unique<Material>(std::vector<Texture*>{ m_Textures["skybox"].get(),
                                                                              m_Textures["defaultNormal"].get(),
                                                                              m_Textures["defaultBlack"].get(),
                                                                              m_Textures["defaultWhite"].get(),
                                                                              m_Textures["defaultWhite"].get() });

    m_Materials["monkey"] = std::make_unique<Material>(std::vector<Texture*>{ m_Textures["monkey_BaseColor"].get(),
                                                                              m_Textures["monkey_Normal"].get(),
                                                                              m_Textures["monkey_Metallic"].get(),
                                                                              m_Textures["monkey_Roughness"].get(),
                                                                              m_Textures["defaultWhite"].get() });

    m_Materials["robot"] =
        std::make_unique<Material>(std::vector<Texture*>{ m_Textures["robot_BaseColor"].get(),
                                                          m_Textures["robot_Normal"].get(),
                                                          m_Textures["robot_Metallic"].get(),
                                                          m_Textures["robot_Roughness"].get(),
                                                          m_Textures["robot_ambientOcclusion"].get() });


    m_Materials["test"] =
        std::make_unique<Material>(std::vector<Texture*>{ m_Textures["subaru_Outside_BaseColor"].get(),
                                                          m_Textures["subaru_Outside_Normal"].get(),
                                                          m_Textures["subaru_Outside_Metallic"].get(),
                                                          m_Textures["subaru_Outside_Roughness"].get(),
                                                          m_Textures["defaultWhite"].get() });

    m_Materials["subaru"] =
        std::make_unique<Material>(std::vector<Texture*>{ m_Textures["subaru_Outside_BaseColor"].get(),
                                                          m_Textures["subaru_Outside_Normal"].get(),
                                                          m_Textures["subaru_Outside_Metallic"].get(),
                                                          m_Textures["subaru_Outside_Roughness"].get(),
                                                          m_Textures["defaultWhite"].get() });

    m_Materials["grid"] = std::make_unique<Material>(std::vector<Texture*>{ m_Textures["uv_grid_3"].get(),
                                                                            m_Textures["defaultNormal"].get(),
                                                                            m_Textures["defaultBlack"].get(),
                                                                            m_Textures["uv_grid"].get(),
                                                                            m_Textures["defaultWhite"].get() });

    m_Materials["gridFancy"] = std::make_unique<Material>(std::vector<Texture*>{ m_Textures["uv_grid_2"].get(),
                                                                                 m_Textures["defaultNormal"].get(),
                                                                                 m_Textures["defaultBlack"].get(),
                                                                                 m_Textures["defaultWhite"].get(),
                                                                                 m_Textures["defaultWhite"].get() });


    m_Materials["fire"] = std::make_unique<Material>(std::vector<Texture*>{ m_Textures["fire_BaseColor"].get(),
                                                                            m_Textures["fire_Normal"].get(),
                                                                            m_Textures["fire_Metallic"].get(),
                                                                            m_Textures["fire_Roughness"].get(),
                                                                            m_Textures["defaultWhite"].get() });

    m_Pipline2D = std::make_unique<Pipeline>(Shader{ "shaders/shader2D.vert.spv", "shaders/shader2D.frag.spv" },
                                             Shader::CreateVertexInputStateInfo<Mesh::Vertex2D>(),
                                             sizeof(UniformBufferObject2D),
                                             sizeof(MeshPushConstants),
                                             std::nullopt,
                                             VK_CULL_MODE_NONE,
                                             VK_FALSE,
                                             VK_FALSE);

    m_Pipline3D = std::make_unique<Pipeline>(Shader{ "shaders/shader3D.vert.spv", "shaders/shader3D.frag.spv" },
                                             Shader::CreateVertexInputStateInfo<Mesh::Vertex3D>(),
                                             sizeof(UniformBufferObject3D),
                                             sizeof(MeshPushConstants),
                                             Material::GetMaterialSetLayout(),
                                             VK_CULL_MODE_NONE);

    m_Pipline3D_Unlit =
        std::make_unique<Pipeline>(Shader{ "shaders/shader3D_Flat.vert.spv", "shaders/shader3D_Flat.frag.spv" },
                                   Shader::CreateVertexInputStateInfo<Mesh::Vertex3D>(),
                                   sizeof(UniformBufferObject3D),
                                   sizeof(MeshPushConstants),
                                   Material::GetMaterialSetLayout(),
                                   VK_CULL_MODE_NONE);

    m_Pipline3D_Cartoon =
        std::make_unique<Pipeline>(Shader{ "shaders/shader3D_Cartoon.vert.spv", "shaders/shader3D_Cartoon.frag.spv" },
                                   Shader::CreateVertexInputStateInfo<Mesh::Vertex3D>(),
                                   sizeof(UniformBufferObject3D),
                                   sizeof(MeshPushConstants),
                                   Material::GetMaterialSetLayout(),
                                   VK_CULL_MODE_NONE);


    const std::vector<Mesh::Vertex2D> triangleVertices = {
        {{ 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f }},
        { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }},
        {{ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }}
    };

    const std::vector<uint32_t> triangleIndeces = { 0, 1, 2 };


    AddMesh2D("Triangle",
              Mesh{
                  triangleIndeces,
                  Mesh::VertexData{.data = (void*)triangleVertices.data(),
                                   .vertexCount = static_cast<uint32_t>(triangleVertices.size()),
                                   .typeSize = sizeof(Mesh::Vertex2D)},
                  nullptr
    });


    const std::vector<Mesh::Vertex2D> squareVertices = {
        {{ -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }},
        { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }},
        {  { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }},
        { { -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }}
    };

    const std::vector<uint32_t> squareIndices = { 0, 1, 2, 0, 2, 3 };

    auto& squareMesh = AddMesh2D("Square",
                                 Mesh{
                                     squareIndices,
                                     Mesh::VertexData{.data = (void*)squareVertices.data(),
                                                      .vertexCount = static_cast<uint32_t>(squareVertices.size()),
                                                      .typeSize = sizeof(Mesh::Vertex2D)},
                                     nullptr
    });


    auto& circleMesh = AddMesh2D("Circle2D", GenerateCircle({ 0, 0 }, { 0.4f, 0.6f }));

    AddMesh3D_Unlit("Skybox", LoadMesh("resources/Skybox/Skybox.obj", m_Materials["skybox"].get()));


    AddMesh3D("Airplane", LoadMesh("resources/Airplane/Airplane.obj", m_Materials["grid"].get()));
    AddMesh3D("Diorama", LoadMesh("resources/Diorama/DioramaGP_2.obj", m_Materials["grid"].get()));
    auto& robot = AddMesh3D("Robot", LoadMesh("resources/Robot/Robot.obj", m_Materials["robot"].get()));
    robot.m_ModelMatrix = translate(glm::mat4(1.0f), glm::vec3(-8.47186, 9.9, -0.140288)) *
                          scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));


    auto& ballMesh = AddMesh3D("Ball", LoadMesh("resources/Primitives/sphere2.obj", m_Materials["gridFancy"].get()));
    ballMesh.m_ModelMatrix = translate(glm::mat4(1.0f), glm::vec3(-1.87057, 6.56198, 6.77501));

    auto& cubeMesh = AddMesh3D("Cube", LoadMesh("resources/Primitives/cube.obj", m_Materials["gridFancy"].get()));
    cubeMesh.m_ModelMatrix = translate(glm::mat4(1.0f), glm::vec3(1.96337, 7.95447, 7.13516));


    auto& monkey = AddMesh3D("Monkey", LoadMesh("resources/Monkey/suzanne_steampunk.obj", m_Materials["monkey"].get()));
    monkey.m_ModelMatrix = translate(glm::mat4(1.0f), glm::vec3(-4.54838, 5.73301, 6.59399));


    auto& carMesh = AddMesh3D("Subaru", LoadMesh("resources/Car/Subaru.obj", m_Materials["subaru"].get()));
    carMesh.m_ModelMatrix = translate(glm::mat4(1.0f), glm::vec3(-6.89595, 4.0f, -0.306714)) *
                            rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) *
                            scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));


    auto& fireMesh = AddMesh3D("Fire", LoadMesh("resources/FireHydrant/fire_hydrant.obj", m_Materials["fire"].get()));
    fireMesh.m_ModelMatrix = translate(glm::mat4(1.0f), glm::vec3(-8.07224, 3.65116, 2.53493)) *
                             scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));

    AddMesh3D_Cartoon("Plane", LoadMesh("resources/Water/Plane.obj", m_Materials["water"].get()));
}

Game::~Game() = default;

void Game::Update()
{
    m_Camera.Update();

    m_Time += jul::GameTime::GetDeltaTimeF() * 1000;

    // Update plane position
    const float planePosition = jul::math::ClampLoop(jul::GameTime::GetElapsedTimeF() * 50.0f, -100.0f, 100.0f);
    m_Meshes3D["Airplane"]->m_ModelMatrix = glm::translate(glm::mat4(1.0f), { 0, 25, planePosition }) *
                                            glm::scale(glm::mat4(1.0f), { 0.01f, 0.01f, 0.01f }) *
                                            glm::rotate(glm::mat4(1.0f), jul::GameTime::GetElapsedTimeF(), { 0, 0, 1 });


    m_Meshes3D_Unlit["Skybox"]->m_ModelMatrix =
        glm::translate(glm::mat4(1.0f), m_Camera.GetPosition()) * scale(glm::mat4(1.0f), glm::vec3(3, 3, 3));
    ;


    m_Meshes2D["Square"]->m_ModelMatrix = translate(glm::mat4(1.0f), glm::vec3(1, 0, 0)) *
                                          glm::rotate(glm::mat4(1.0f), jul::GameTime::GetElapsedTimeF(), { 0, 0, 1 });
    m_Meshes2D["Triangle"]->m_ModelMatrix = glm::rotate(glm::mat4(1.0f), jul::GameTime::GetElapsedTimeF(), { 0, 0, 1 });
    m_Meshes2D["Circle2D"]->m_ModelMatrix = translate(glm::mat4(1.0f), glm::vec3(-1, 0, 0)) *
                                            glm::rotate(glm::mat4(1.0f), jul::GameTime::GetElapsedTimeF(), { 0, 0, 1 });


    if(Input::GetKeyHeld(GLFW_KEY_1))
        m_RenderMode = 0;
    if(Input::GetKeyHeld(GLFW_KEY_2))
        m_RenderMode = 1;
    if(Input::GetKeyHeld(GLFW_KEY_3))
        m_RenderMode = 2;
    if(Input::GetKeyHeld(GLFW_KEY_4))
        m_RenderMode = 3;
    if(Input::GetKeyHeld(GLFW_KEY_5))
        m_RenderMode = 4;
    if(Input::GetKeyHeld(GLFW_KEY_6))
        m_RenderMode = 5;
    if(Input::GetKeyHeld(GLFW_KEY_7))
        m_RenderMode = 6;
}

void Game::Draw(VkCommandBuffer commandBuffer, int imageIndex)
{
    {
        auto projectionMatrix = m_Camera.GetProjectionMatrix();
        projectionMatrix[1][1] *= -1;

        UniformBufferObject3D ubo3D{ .viewProjection = projectionMatrix * m_Camera.GetViewMatrix(),
                                     .viewPosition = glm::vec4(m_Camera.GetPosition(), 1.0f),
                                     .renderMode = m_RenderMode };


        m_Pipline3D_Unlit->Bind(commandBuffer, imageIndex);
        m_Pipline3D_Unlit->UpdateUBO(imageIndex, &ubo3D, sizeof(ubo3D));

        for(auto&& mesh : m_Meshes3D_Unlit)
        {
            MeshPushConstants meshPushConstant{};
            {
                meshPushConstant.model = mesh.second->m_ModelMatrix;
            }
            m_Pipline3D_Unlit->UpdatePushConstant(commandBuffer, &meshPushConstant, sizeof(meshPushConstant));


            // Update Material
            Material* material = mesh.second->GetMaterial();
            if(material != nullptr)
                m_Pipline3D_Unlit->UpdateMaterial(commandBuffer, *material);

            // Draw mesh
            mesh.second->Draw(commandBuffer);
        }
    }

    UniformBufferObject2D ubo2D{};
    {
        ubo2D.proj = m_Camera.GetOrthoProjectionMatrix();
    }
    m_Pipline2D->Bind(commandBuffer, imageIndex);
    m_Pipline2D->UpdateUBO(imageIndex, &ubo2D, sizeof(ubo2D));

    for(auto&& mesh : m_Meshes2D)
    {

        MeshPushConstants meshPushConstant{};
        {
            meshPushConstant.model = mesh.second->m_ModelMatrix;
        }
        m_Pipline2D->UpdatePushConstant(commandBuffer, &meshPushConstant, sizeof(meshPushConstant));


        // Update Material
        Material* material = mesh.second->GetMaterial();
        if(material != nullptr)
            m_Pipline2D->UpdateMaterial(commandBuffer, *material);

        // Draw mesh
        mesh.second->Draw(commandBuffer);
    }

    {
        auto projectionMatrix = m_Camera.GetProjectionMatrix();
        projectionMatrix[1][1] *= -1;

        UniformBufferObject3D ubo3D{ .viewProjection = projectionMatrix * m_Camera.GetViewMatrix(),
                                     .viewPosition = glm::vec4(m_Camera.GetPosition(), 1.0f),
                                     .renderMode = m_RenderMode };


        m_Pipline3D->Bind(commandBuffer, imageIndex);
        m_Pipline3D->UpdateUBO(imageIndex, &ubo3D, sizeof(ubo3D));

        for(auto&& mesh : m_Meshes3D)
        {
            MeshPushConstants meshPushConstant{};
            {
                meshPushConstant.model = mesh.second->m_ModelMatrix;
            }
            m_Pipline3D->UpdatePushConstant(commandBuffer, &meshPushConstant, sizeof(meshPushConstant));


            // Update Material
            Material* material = mesh.second->GetMaterial();
            if(material != nullptr)
                m_Pipline3D->UpdateMaterial(commandBuffer, *material);

            // Draw mesh
            mesh.second->Draw(commandBuffer);
        }
    }

    {
        auto projectionMatrix = m_Camera.GetProjectionMatrix();
        projectionMatrix[1][1] *= -1;

        UniformBufferObject3D ubo3D{ .viewProjection = projectionMatrix * m_Camera.GetViewMatrix(),
                                     .viewPosition = glm::vec4(m_Camera.GetPosition(), 1.0f),
                                     .renderMode = (int)m_Time };


        m_Pipline3D_Cartoon->Bind(commandBuffer, imageIndex);
        m_Pipline3D_Cartoon->UpdateUBO(imageIndex, &ubo3D, sizeof(ubo3D));

        for(auto&& mesh : m_Meshes3D_Cartoon)
        {
            MeshPushConstants meshPushConstant{};
            {
                meshPushConstant.model = mesh.second->m_ModelMatrix;
            }
            m_Pipline3D_Cartoon->UpdatePushConstant(commandBuffer, &meshPushConstant, sizeof(meshPushConstant));


            // Update Material
            Material* material = mesh.second->GetMaterial();
            if(material != nullptr)
                m_Pipline3D_Cartoon->UpdateMaterial(commandBuffer, *material);

            // Draw mesh
            mesh.second->Draw(commandBuffer);
        }
    }
}

void Game::OnResize() { m_Camera.SetAspect(VulkanGlobals::GetSwapChain().GetAspect()); }

// float Cross(const glm::vec2& v1, const glm::vec2& v2) { return v1.x * v2.y - v1.y * v2.x; }

void ComputeTangents(std::vector<Mesh::Vertex3D>& vertices, const std::vector<uint32_t>& indices)
{
    std::vector<glm::vec3> tan1(vertices.size());
    std::vector<glm::vec3> tan2(vertices.size());
    for(size_t i = 0; i < indices.size(); i += 3)
    {
        const Mesh::Vertex3D& vertex0 = vertices[indices[i]];
        const Mesh::Vertex3D& vertex1 = vertices[indices[i + 1]];
        const Mesh::Vertex3D& vertex2 = vertices[indices[i + 2]];

        const glm::vec3 dPos1 = vertex1.position - vertex0.position;
        const glm::vec3 dPos2 = vertex2.position - vertex0.position;

        const glm::vec2 dUV1 = vertex1.uv - vertex0.uv;
        const glm::vec2 dUV2 = vertex2.uv - vertex0.uv;

        const float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

        const glm::vec3 uDir = (dPos1 * dUV2.y - dPos2 * dUV1.y) * r;
        const glm::vec3 vDir = (dPos2 * dUV1.x - dPos1 * dUV2.x) * r;

        tan1[indices[i]] += uDir;
        tan1[indices[i + 1]] += uDir;
        tan1[indices[i + 2]] += uDir;

        tan2[indices[i]] += vDir;
        tan2[indices[i + 1]] += vDir;
        tan2[indices[i + 2]] += vDir;
    }

    for(size_t i = 0; i < vertices.size(); i++)
    {
        const glm::vec3 n = vertices[i].normal;
        const glm::vec3 t = tan1[i];

        const glm::vec3 tangent = glm::normalize(t - n * glm::dot(n, t));
        const float tangentHandedness = (glm::dot(glm::cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f;

        vertices[i].tangent = { tangent.x, tangent.y, tangent.z, tangentHandedness };
    }
}

Mesh Game::LoadMesh(const std::string& meshPath, Material* material)
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
    std::vector<uint32_t> indices{};


    std::unordered_map<Mesh::Vertex3D, uint32_t> uniqueVertices{};

    for(auto&& shape : shapes)
    {
        for(auto&& index : shape.mesh.indices)
        {
            const Mesh::Vertex3D vertex{
                .position = { attrib.vertices[3 * index.vertex_index + 0],
                             attrib.vertices[3 * index.vertex_index + 1],
                             attrib.vertices[3 * index.vertex_index + 2] },
                .normal = { attrib.normals[3 * index.normal_index + 0],
                             attrib.normals[3 * index.normal_index + 1],
                             attrib.normals[3 * index.normal_index + 2] },
                .tangent = { glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) },
                .uv = { attrib.texcoords[2 * index.texcoord_index + 0],
                             1.0f - attrib.texcoords[2 * index.texcoord_index + 1] }
            };

            if(not uniqueVertices.contains(vertex))
            {
                uniqueVertices[vertex] = vertices.size();
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }

    ComputeTangents(vertices, indices);

    return Mesh{
        indices,
        Mesh::VertexData{.data = (void*)vertices.data(),
                         .vertexCount = static_cast<uint32_t>(vertices.size()),
                         .typeSize = sizeof(Mesh::Vertex3D)},
        material
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
                         .typeSize = sizeof(Mesh::Vertex2D)},
        nullptr
    };
}
