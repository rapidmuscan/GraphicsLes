#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:

#define modulus_value 50.0
	double maxim_elements ;
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void MakePartMove(int n);
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* ParticleShader;
	double msec2;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* Particles;
	Light* light;
	Camera* camera;

	GLuint cubeMap;

	float waterRotate;

};
