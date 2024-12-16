#pragma once

#include "../graphics/shader.h"
#include "../graphics/texture.h"
#include "../graphics/buffer.h"
#include "../scene.h"

class TessellationScene : public Scene
{
public:
	TessellationScene();

	void setup();
	void clean();

	void update(float deltaTime);
	void render(const Camera& camera, float deltaTime);

	void processGUI();

private:
	ShaderProgram* renderMeshShader;

	Texture* heightMapTex;

	VAO* meshVAO;
	VBO* meshVBO;
	IBO* meshIBO;

	uint32_t numStrips;
	uint32_t numVerticesPerStrip;

	bool renderWireframe;
};
