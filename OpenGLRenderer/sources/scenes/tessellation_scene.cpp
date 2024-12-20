#include "tessellation_scene.h"

TessellationScene::TessellationScene()
	: renderMeshShader(nullptr), heightMapTex(nullptr),
      meshVAO(nullptr), meshVBO(nullptr),
      numPatches(20),
      renderWireframe(false)
{
}

void TessellationScene::setup()
{
    std::vector<float> vertices;
    int maxTessellationLevel, width, height;

    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessellationLevel);

    renderMeshShader = new ShaderProgram("sources/shaders/11_render_mesh_vs.glsl", "sources/shaders/11_render_mesh_tcs.glsl", "sources/shaders/11_render_mesh_tes.glsl", "sources/shaders/11_render_mesh_fs.glsl");

    heightMapTex = new Texture("resources/textures/iceland_heightmap.png");

    heightMapTex->bind(0);

    width = heightMapTex->getWidth();
    height = heightMapTex->getHeight();

    for (uint32_t i = 0; i <= numPatches - 1; i++)
    {
        for (uint32_t j = 0; j <= numPatches - 1; j++)
        {
            vertices.push_back(-width / 2.0f + width * i / float(numPatches)); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / float(numPatches)); // v.z
            vertices.push_back(i / float(numPatches)); // u
            vertices.push_back(j / float(numPatches)); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / float(numPatches)); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * j / float(numPatches)); // v.z
            vertices.push_back((i + 1) / float(numPatches)); // u
            vertices.push_back(j / float(numPatches)); // v

            vertices.push_back(-width / 2.0f + width * i / float(numPatches)); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / float(numPatches)); // v.z
            vertices.push_back(i / float(numPatches)); // u
            vertices.push_back((j + 1) / float(numPatches)); // v

            vertices.push_back(-width / 2.0f + width * (i + 1) / float(numPatches)); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height / 2.0f + height * (j + 1) / float(numPatches)); // v.z
            vertices.push_back((i + 1) / float(numPatches)); // u
            vertices.push_back((j + 1) / float(numPatches)); // v
        }
    }

    meshVAO = new VAO();
    meshVBO = new VBO(&vertices[0], vertices.size() * sizeof(float));

    meshVAO->bind();
    meshVBO->bind();

    meshVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    meshVAO->setVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    meshVAO->unbind(); // Unbind VAO before another buffer.
    meshVBO->unbind();

    std::cout << "[LOG] Number of vertices generated: " << vertices.size() << std::endl;
    std::cout << "[LOG] Max available tessellation level: " << maxTessellationLevel << std::endl;

    glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void TessellationScene::clean()
{
    renderMeshShader->clean();
    heightMapTex->clean();
    meshVAO->clean();
    meshVBO->clean();

    delete renderMeshShader;
    delete heightMapTex;
    delete meshVAO;
    delete meshVBO;
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

    renderMeshShader->setUniform1i("uHeightMap", 0);

    glDrawArrays(GL_PATCHES, 0, 4 * numPatches * numPatches);

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
