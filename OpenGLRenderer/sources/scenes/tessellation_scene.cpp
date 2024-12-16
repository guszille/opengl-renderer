#include "tessellation_scene.h"

TessellationScene::TessellationScene()
	: renderMeshShader(nullptr), heightMapTex(nullptr),
      meshVAO(nullptr), meshVBO(nullptr), meshIBO(nullptr),
      numStrips(0), numVerticesPerStrip(0),
      renderWireframe(false)
{
}

void TessellationScene::setup()
{
    float yScale = 64.0f / 256.0f, yShift = 16.0f; // Apply a scale + shift to the height data.
    int width, height, colorChannels;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    stbi_set_flip_vertically_on_load(true);
    stbi_uc* heightMap = stbi_load("resources/textures/iceland_heightmap.png", &width, &height, &colorChannels, 0);

    if (heightMap)
    {
        std::cout << "[LOG] Loaded height map of size (" << width << ", " << height << ")." << std::endl;
    }
    else
    {
        std::cerr << "[ERROR] Failed to load texture height map." << std::endl;
    }

    for (uint32_t i = 0; i < height; i++)
    {
        for (uint32_t j = 0; j < width; j++)
        {
            stbi_uc* texel = heightMap + (j + width * i) * colorChannels;

            if (texel)
            {
                int y = static_cast<int>(texel[0]);

                vertices.push_back(height * i / float(height) - height / 2.0f);
                vertices.push_back(y * yScale - yShift);
                vertices.push_back(width * j / float(width) - width / 2.0f);
            }
        }
    }

    for (uint32_t i = 0; i < height - 1; i++)
    {
        for (uint32_t j = 0; j < width; j++)
        {
            for (uint32_t k = 0; k < 2; k++)
            {
                indices.push_back(j + width * (i + k));
            }
        }
    }

    renderMeshShader = new ShaderProgram("sources/shaders/11_render_mesh_vs.glsl", "sources/shaders/11_render_mesh_fs.glsl");

    meshVAO = new VAO();
    meshVBO = new VBO(&vertices[0], vertices.size() * sizeof(float));
    meshIBO = new IBO(&indices[0], indices.size() * sizeof(uint32_t));

    meshVAO->bind();
    meshVBO->bind();
    meshIBO->bind();

    meshVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

    meshVAO->unbind(); // Unbind VAO before another buffer.
    meshVBO->unbind();
    meshIBO->unbind();

    numStrips = height - 1;
    numVerticesPerStrip = width * 2 - 2;

    stbi_image_free(heightMap);

    std::cout << "[LOG] Loaded " << vertices.size() / 3 << " vertices." << std::endl;
    std::cout << "[LOG] Loaded " << indices.size() << " indices." << std::endl;
    std::cout << "[LOG] Created lattice of " << numStrips << " strips with " << numVerticesPerStrip << " triangles each." << std::endl;
    std::cout << "[LOG] Created " << numStrips * numVerticesPerStrip << " triangles total." << std::endl;

    glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void TessellationScene::clean()
{
    renderMeshShader->clean();
    //heightMapTex->clean();
    meshVAO->clean();
    meshVBO->clean();
    meshIBO->clean();

    delete renderMeshShader;
    //delete heightMapTex;
    delete meshVAO;
    delete meshVBO;
    delete meshIBO;
}

void TessellationScene::update(float deltaTime)
{
}

void TessellationScene::render(const Camera& camera, float deltaTime)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (renderWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -15.0f, 0.0f));

    meshVAO->bind();
    renderMeshShader->bind();

    renderMeshShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
    renderMeshShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
    renderMeshShader->setUniformMatrix4fv("uModelMatrix", modelMatrix);

    for (uint32_t strip = 0; strip < numStrips; strip++)
    {
        glDrawElements(GL_TRIANGLE_STRIP, numVerticesPerStrip + 2, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * (numVerticesPerStrip + 2) * strip));
    }

    renderMeshShader->unbind();
    meshVAO->unbind();
}

void TessellationScene::processGUI()
{
    bool dialogOpen = true;
    ImGui::Begin("Tessellation Dialog", &dialogOpen, ImGuiWindowFlags_MenuBar);

    ImGui::SeparatorText("Mesh");

    ImGui::Checkbox("Render Wireframe", &renderWireframe);

    ImGui::End();
}
