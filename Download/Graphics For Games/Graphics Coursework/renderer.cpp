#include"Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera();
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	quad = Mesh::GenerateQuad();
	Particles = Mesh::Particles();


	camera->SetPosition(Vector3(0, 0, 0));

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)), Vector4(1.0f, 1.0f, 1.0f, 1), (RAW_WIDTH * HEIGHTMAP_X));

	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR"reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl",
		SHADERDIR"skyboxFragment.glsl");
	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR"PerPixelFragment.glsl");
	ParticleShader = new Shader(SHADERDIR "MatrixVertex.glsl",
		SHADERDIR "colourFragment.glsl");


	if (!reflectShader->LinkProgram() || !lightShader->LinkProgram() ||
		!skyboxShader->LinkProgram()) {
		return;	}	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"lava.TGA",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"icyhell_ft.jpg", TEXTUREDIR"icyhell_bk.jpg",
		TEXTUREDIR"icyhell_up.jpg", TEXTUREDIR"icyhell_dn.jpg",
		TEXTUREDIR"icyhell_rt.jpg", TEXTUREDIR"icyhell_lf.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);
	

	if (!cubeMap || !quad->GetTexture() || !heightMap->GetTexture() ||
		!heightMap->GetBumpMap()) {
		return;

	}

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);	init = true;
	waterRotate = 0.0f;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f*20,
		(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}
Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;
	currentShader = 0;

}
void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 4000.0f;
}void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_8))
	{
		MakePartMove(8);
	}
	else if (Window::GetKeyboard()->KeyDown(KEYBOARD_6))
	{
		MakePartMove(2);
	}
	else if (Window::GetKeyboard()->KeyDown(KEYBOARD_4))
	{
		MakePartMove(4);
	}


	DrawSkybox();
	DrawHeightmap();
	DrawWater();

	SetCurrentShader(ParticleShader);

	glUseProgram(currentShader->GetProgram());

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)& projMatrix);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)& viewMatrix);

	

	modelMatrix = Matrix4::Translation(Vector3(0,100,0)) * Matrix4::Scale(Vector3(1000, 1000, 1000));

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& modelMatrix);

	Particles->Draw();
	
	SwapBuffers();
}void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}void Renderer::DrawHeightmap() {
	


	SetCurrentShader(lightShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();
	heightMap->Draw();
	
	glUseProgram(0);

}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 150 * HEIGHTMAP_Y / 3.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 100.0f));


	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);

}

void Renderer::MakePartMove(int n)
{
	Particles->ParticalsMove(n);
}
