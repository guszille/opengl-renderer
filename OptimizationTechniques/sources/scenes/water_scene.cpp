#include "water_scene.h"

WaterScene::WaterScene()
    : Scene(), renderSkyBoxShader(nullptr), renderWaterShader(nullptr), renderStaticModelShader(nullptr),
      skyBoxCM(nullptr), skyBoxVAO(nullptr), skyBoxVBO(nullptr),
      waterMeshVAO(nullptr), waterMeshVBO(nullptr),
      reflectionFBWidth(1280), reflectionFBHeight(720), refractionFBWidth(1280), refractionFBHeight(720), reflectionFB(nullptr), refractionFB(nullptr),
      marsModel(nullptr), craterModel(nullptr),
      debugQuadRenderer(nullptr),
      waterPosition(0.0f, 0.0f, 0.0f), terrainPosition(0.0f, -1.0f, 0.0f),
      time(0.0f)
{
}

void WaterScene::setup()
{
    float skyBoxVertices[] = {
        // positions and uvs         
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    float planeVertices[] = {
        // positions         // normals           // uvs
         0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        -0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        -0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f
    };

    // Setup shaders.
    renderSkyBoxShader = new ShaderProgram("sources/shaders/10_render_skybox_vs.glsl", "sources/shaders/10_render_skybox_fs.glsl");
	renderWaterShader = new ShaderProgram("sources/shaders/10_render_water_vs.glsl", "sources/shaders/10_render_water_fs.glsl");
    renderStaticModelShader = new ShaderProgram("sources/shaders/10_render_static_model_vs.glsl", "sources/shaders/10_render_static_model_fs.glsl");

    // Setup skybox cubemap.
    std::array<const char*, 6> skyBoxFaces = {
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
    };

    skyBoxCM = new CubeMap(skyBoxFaces);

    // Setup skybox buffers.
    skyBoxVAO = new VAO();
    skyBoxVBO = new VBO(skyBoxVertices, sizeof(skyBoxVertices));

    skyBoxVAO->bind();
    skyBoxVBO->bind();

    skyBoxVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

    skyBoxVAO->unbind(); // Unbind VAO before another buffer.
    skyBoxVBO->unbind();

    // Setup water mesh buffers.
    waterMeshVAO = new VAO();
    waterMeshVBO = new VBO(planeVertices, sizeof(planeVertices));

    waterMeshVAO->bind();
    waterMeshVBO->bind();

    waterMeshVAO->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    waterMeshVAO->setVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    waterMeshVAO->setVertexAttribute(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    waterMeshVAO->unbind(); // Unbind VAO before another buffer.
    waterMeshVBO->unbind();

    // Setup framebuffers.
    reflectionFB = new FrameBuffer(reflectionFBWidth, reflectionFBHeight, 1, GL_RGB, GL_LINEAR, GL_REPEAT);
    refractionFB = new FrameBuffer(refractionFBWidth, refractionFBHeight, 1, GL_RGB, GL_LINEAR, GL_REPEAT);

    // Setup models.
    uint32_t modelLoaderFlags = aiProcess_Triangulate | aiProcess_GenNormals;

    marsModel = new Model("resources/models/mars/mars.obj", modelLoaderFlags);
    craterModel = new Model("resources/models/crater/moon_crater.gltf", modelLoaderFlags);

    // Setup debug tools.
    debugQuadRenderer = new QuadRenderer();

    debugQuadRenderer->setup();

    // Debug.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void WaterScene::clean()
{
    renderSkyBoxShader->clean();
    renderWaterShader->clean();
    renderStaticModelShader->clean();
    skyBoxCM->clean();
    skyBoxVAO->clean();
    skyBoxVBO->clean();
    waterMeshVAO->clean();
    waterMeshVBO->clean();
    reflectionFB->clean();
    refractionFB->clean();
    marsModel->clean();
    craterModel->clean();
    debugQuadRenderer->clean();

    delete renderSkyBoxShader;
    delete renderWaterShader;
    delete renderStaticModelShader;
    delete skyBoxCM;
    delete skyBoxVAO;
    delete skyBoxVBO;
    delete waterMeshVAO;
    delete waterMeshVBO;
    delete reflectionFB;
    delete refractionFB;
    delete marsModel;
    delete craterModel;
    delete debugQuadRenderer;
}

void WaterScene::update(float deltaTime)
{
    time += deltaTime;
}

void WaterScene::render(const Camera& camera, float deltaTime)
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport); // Save current viewport.

    glViewport(0, 0, reflectionFBWidth, reflectionFBHeight);

    glm::vec3 reflectionCameraPos = glm::vec3(camera.getPosition().x, -1.0f * camera.getPosition().y, camera.getPosition().z);
    glm::vec3 reflectionCameraDir = glm::normalize(glm::reflect(camera.getDirection(), glm::vec3(0.0f, 1.0f, 0.0f)));

    Camera reflectionCamera(reflectionCameraPos, reflectionCameraDir, glm::vec3(0.0f, 1.0f, 0.0f), { float(reflectionFBWidth) / float(reflectionFBHeight) });

    reflectionFB->bind();
    renderScene(reflectionCamera, deltaTime, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    reflectionFB->unbind();

    glViewport(0, 0, refractionFBWidth, refractionFBHeight);

    refractionFB->bind();
    renderScene(camera, deltaTime, glm::vec4(0.0f, -1.0f, 0.0f, 0.0f));
    refractionFB->unbind();

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    renderScene(camera, deltaTime);

    reflectionFB->bindColorBuffer(0);
    refractionFB->bindColorBuffer(1);

    debugQuadRenderer->render(0, 3, 16, 16, 256, 144);
    debugQuadRenderer->render(1, 3, viewport[2] - 16 - 256, 16, 256, 144);

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void WaterScene::processGUI()
{
    bool dialogOpen = true;
    ImGui::Begin("Water Dialog", &dialogOpen, ImGuiWindowFlags_MenuBar);

    ImGui::SeparatorText("Terrain");

    ImGui::DragFloat3("Position", glm::value_ptr(terrainPosition), 0.1f, -10.0f, 10.0f, "%.1f");

    ImGui::End();
}

void WaterScene::renderScene(const Camera& camera, float deltaTime, const glm::vec4& clipPlane)
{
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (glm::length(clipPlane) == 0.0f)
    {
        glDisable(GL_CLIP_DISTANCE0);
    }
    else
    {
        glEnable(GL_CLIP_DISTANCE0);
    }

    // Render models.
    renderStaticModelShader->bind();

    renderStaticModelShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
    renderStaticModelShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());

    renderStaticModelShader->setUniform3f("uViewPos", camera.getPosition());

    renderStaticModelShader->setUniform3f("uLight.ambient", glm::vec3(1.0f));
    renderStaticModelShader->setUniform3f("uLight.diffuse", glm::vec3(0.0f)); // Disabled.
    renderStaticModelShader->setUniform3f("uLight.specular", glm::vec3(0.0f)); // Disabled.
    renderStaticModelShader->setUniform3f("uLight.position", glm::vec3(0.0f));

    renderStaticModelShader->setUniform1f("uMaterial.shininess", 64.0f);

    renderStaticModelShader->setUniform4f("uClipPlane", clipPlane);

    // Render mars model.
    {
        glm::mat4 marsModelMatrix(1.0f);
        marsModelMatrix = glm::translate(marsModelMatrix, glm::vec3(0.0f, 15.0f, -15.0f));

        renderStaticModelShader->setUniformMatrix4fv("uModelMatrix", marsModelMatrix);

        marsModel->render(renderStaticModelShader);
    }

    // Render crater model.
    {
        glm::mat4 craterModelMatrix(1.0f);
        craterModelMatrix = glm::translate(craterModelMatrix, terrainPosition);
        craterModelMatrix = glm::rotate(craterModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        craterModelMatrix = glm::scale(craterModelMatrix, glm::vec3(100.0f));

        renderStaticModelShader->setUniformMatrix4fv("uModelMatrix", craterModelMatrix);

        craterModel->render(renderStaticModelShader);
    }

    renderStaticModelShader->unbind();

    // Render water.
    renderWaterShader->bind();
    waterMeshVAO->bind();

    reflectionFB->bindColorBuffer(0);
    refractionFB->bindColorBuffer(1);

    glm::mat4 waterModelMatrix(1.0f);
    waterModelMatrix = glm::scale(waterModelMatrix, glm::vec3(200.0f, 0.0f, 200.0f));

    renderWaterShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
    renderWaterShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
    renderWaterShader->setUniformMatrix4fv("uModelMatrix", waterModelMatrix);

    renderWaterShader->setUniform1i("uReflectionTex", 0);
    renderWaterShader->setUniform1i("uRefractionTex", 1);

    if (glm::length(clipPlane) == 0.0f)
    {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    waterMeshVAO->unbind();
    renderWaterShader->unbind();

    // Render skybox.
    renderSkyBoxShader->bind();
    skyBoxVAO->bind();

    skyBoxCM->bind(0);

    renderSkyBoxShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
    renderSkyBoxShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());

    renderSkyBoxShader->setUniform1i("uCubeMap", 0);

    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);

    skyBoxVAO->unbind();
    renderSkyBoxShader->unbind();
}
