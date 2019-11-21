#include"Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera();
	maxim_elements = 5000.0;
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	quad = Mesh::GenerateQuad();
	Particles = Mesh::Particles();


	camera->SetPosition(Vector3(0, 3000, 0));

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

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f*20,(float)width / (float)height, 45.0f);

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
	waterRotate += msec / 4000.0f;	}void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	MakePartMove(1);

	DrawSkybox();
	DrawHeightmap();
	DrawWater();

	SetCurrentShader(ParticleShader);

	glUseProgram(currentShader->GetProgram());

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)& projMatrix);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)& viewMatrix);
	
	modelMatrix = Matrix4::Translation(Vector3(0, 12000, 0)) * Matrix4::Scale(Vector3(1000, 1000, 1000));

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& modelMatrix);
	
	projMatrix = Matrix4::Orthographic(+1, 1, 1, -1, 1, -1);
	
	
	int widthmapx = (HEIGHTMAP_X / 2) * (1000 / 10) * 10;
	Vector3 tempposition = Vector3(0, 40000, 0);
	for (int l = 0; l < widthmapx; l += widthmapx / 25)
	{
		for (int j = 0; j < widthmapx; j += widthmapx / 25)
		{
			for (int i = 0; i < widthmapx; i += widthmapx / 25)
			{
				modelMatrix = Matrix4::Translation(tempposition) * Matrix4::Scale(Vector3(100, 100, 100));
				glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& modelMatrix);
				Particles->Draw();
				tempposition.x += widthmapx / 25;

			}
			tempposition.x = 0;
			tempposition.z += widthmapx / 25;
		}
		tempposition.z = 0;
		tempposition.y += widthmapx / 25;
	}
	//for (int i = 0; i < (HEIGHTMAP_X / 2) * (1000 / 10) * 5; i += (HEIGHTMAP_X / 2) * (1000 / 10))
	//{
	//	tempposition.x += (HEIGHTMAP_X / 2)*(1000/10)*2;
	//	modelMatrix = Matrix4::Translation(tempposition) * Matrix4::Scale(Vector3(1000, 1000, 1000));
	//	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& modelMatrix);
	//	Particles->Draw();
	//}

	//for (int i = 0; i < (HEIGHTMAP_X / 2) * (1000 / 10) * 5; i += (HEIGHTMAP_X / 2) * (1000 / 10))
	//{
	//	tempposition.z += (HEIGHTMAP_X / 2) * (1000 / 10) * 2;
	//	modelMatrix = Matrix4::Translation(tempposition) * Matrix4::Scale(Vector3(1000, 1000, 1000));
	//	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& modelMatrix);
	//	Particles->Draw();

	//}
	//
	//tempposition = Vector3(0, 12000, 0);
	//
	//for (int i = 0; i < (HEIGHTMAP_X / 2) * (1000 / 10) * 5; i += (HEIGHTMAP_X / 2) * (1000 / 10))
	//{
	//	tempposition.z += (HEIGHTMAP_X / 2) * (1000 / 10) * 2;
	//	modelMatrix = Matrix4::Translation(tempposition) * Matrix4::Scale(Vector3(1000, 1000, 1000));
	//	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& modelMatrix);
	//	Particles->Draw();
	//}

	//for (int i = 0; i < (HEIGHTMAP_X / 2) * (1000 / 10) * 5; i += (HEIGHTMAP_X / 2) * (1000 / 10))
	//{
	//	tempposition.x += (HEIGHTMAP_X / 2) * (1000 / 10) * 2;
	//	modelMatrix = Matrix4::Translation(tempposition) * Matrix4::Scale(Vector3(1000, 1000, 1000));
	//	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& modelMatrix);
	//	Particles->Draw();

	//}

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f * 20, (float)width / (float)height, 45.0f);

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
