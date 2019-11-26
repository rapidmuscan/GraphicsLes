#pragma once
//Made By Alexander Sablin 180575673
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"

# define POST_PASSES 10

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
	void DrawParticles();
	void MakePartMove(int n);
	void PressKeyFunction();
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* ParticleShader;
	double msec2;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* postQuad;
	Mesh* viewport;
	Mesh* Particles;
	Mesh* ParticlesSky;
	Light* light;
	Camera* camera;

	bool Patricalmove = true;
	bool Autocameraon = true;
	bool Killyoureyesmode = false;
	float blurval = 0.1;


	GLuint cubeMap;
	bool changewor = false;
	float waterRotate;
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();
	Shader* sceneShader;
	Shader * processShader;
	float texchange = 0;
	float totaltimechange;
	float timePassed;
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
	

	void combineBuffers();
};
