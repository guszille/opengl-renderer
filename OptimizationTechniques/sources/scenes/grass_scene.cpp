#include "grass_scene.h"

float quadVertices[] = {
    // positions         // uvs
    -0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // bottom-left
     0.5f, -0.5f,  0.0f,  0.0f,  0.0f, // bottom-right
     0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-right
     0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-right
    -0.5f,  0.5f,  0.0f,  1.0f,  1.0f, // top-left
    -0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // bottom-left
};

GrassScene::GrassScene()
	: Scene(), grassRenderShader(nullptr), vao(nullptr), vbo(nullptr), instanceMatrices(nullptr), texture(nullptr), modelMatrices(nullptr), instances(400), time()
{
}

void GrassScene::setup()
{
    std::size_t vec4_s = sizeof(glm::vec4);
    int axisLim = int(std::sqrtf(float(instances)));
    int axisOffset = axisLim / 2;

	grassRenderShader = new ShaderProgram("sources/shaders/render_grass_vs.glsl", "sources/shaders/render_grass_fs.glsl");

	modelMatrices = new glm::mat4[instances];
    
    for (int x = 0; x < axisLim; ++x)
    {
        for (int z = 0; z < axisLim; ++z)
        {
            // float xOffset = static_cast<float>(rand() % 1) - 0.5f;
            // float zOffset = static_cast<float>(rand() % 1) - 0.5f;

            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 position(x - axisOffset, 0.0f, z - axisOffset);
            int index = x * axisLim + z;

            model = glm::translate(model, position);

            modelMatrices[index] = model;
        }
    }

    vao = new VAO();
    vbo = new VBO(quadVertices, sizeof(quadVertices));
    instanceMatrices = new VBO(&modelMatrices[0], instances * sizeof(glm::mat4));

    vao->bind();
    vbo->bind();

    vao->setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    vao->setVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    instanceMatrices->bind();

    vao->setVertexAttribute(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(0), 1);
    vao->setVertexAttribute(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(1 * vec4_s), 1);
    vao->setVertexAttribute(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(2 * vec4_s), 1);
    vao->setVertexAttribute(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4_s, (void*)(3 * vec4_s), 1);

    vao->unbind(); // Unbind VAO before another buffer.
    vbo->unbind();
    instanceMatrices->unbind();

	texture = new Texture("resources/textures/grass.png");
}

void GrassScene::clean()
{
    grassRenderShader->clean();
    vao->clean();
    vbo->clean();
    instanceMatrices->clean();
    texture->clean();

    delete grassRenderShader;
    delete vao;
    delete vbo;
    delete instanceMatrices;
    delete texture;

    delete[] modelMatrices;
}

void GrassScene::update(float deltaTime)
{
    if (time + deltaTime >= 360.0f)
    {
        time = 0.0f;
    }
    else
    {
        time += deltaTime;
    }
}

void GrassScene::render(const Camera& camera, float deltaTime)
{
    glm::vec4 wind(glm::normalize(glm::vec3(0.5f, 0.0f, 1.0f)), time * 0.005f); // Wind direction (XYZ) and intensity (W).

    glClearColor(0.75f, 0.5f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grassRenderShader->bind();
    texture->bind(0);
    vao->bind();

    grassRenderShader->setUniformMatrix4fv("uProjectionMatrix", camera.getProjectionMatrix());
    grassRenderShader->setUniformMatrix4fv("uViewMatrix", camera.getViewMatrix());
    grassRenderShader->setUniform1i("uTexture", 0);
    grassRenderShader->setUniform4f("uWind", wind);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instances);

    vao->unbind();
    texture->unbind();
    grassRenderShader->unbind();
}
