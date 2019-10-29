#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	triangle = Mesh::GenerateTriangle();
	triangle2 = Mesh::GenerateTriangle2();

	currentShader = new Shader(SHADERDIR"basicVertex.glsl",
		SHADERDIR"colourFragment.glsl");

	if(!currentShader->LinkProgram()) {
		return;
	}

	init = true;
}
Renderer::~Renderer(void)	{
	delete triangle;
	delete triangle2;
}

void Renderer::RenderScene()	{
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);	

	glUseProgram(currentShader->GetProgram());
	triangle->Draw();
	triangle2->Draw();
	glUseProgram(0);

	SwapBuffers();	
}

