#include "../../nclgl/window.h"
#include"Renderer.h"

//Made By Alexander Sablin 180575673
Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	totaltimechange = 10.0f;
	timePassed = 0.0f;
	camera = new Camera();
	maxim_elements = 5000.0;
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	quad = Mesh::GenerateQuad();
	postQuad = Mesh::GenerateQuad();
	Particles = Mesh::Particles();
	ParticlesSky = Mesh::ParticlesSky();

	viewport = Mesh::GenerateQuad();


	camera->SetPosition(Vector3(0, 3000, 0));

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 30000.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)), Vector4(1.0f, 0.5f, 0.5f, 1), ((HEIGHTMAP_X / 2) * (1000 / 10) * 10));

	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl",	SHADERDIR"skyboxFragment.glsl");
	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"PerPixelFragment.glsl");
	ParticleShader = new Shader(SHADERDIR "MatrixVertex.glsl", SHADERDIR "colourFragment.glsl");
	sceneShader = new Shader(SHADERDIR "TexturedVertex.glsl",SHADERDIR "TexturedFragment.glsl");
	processShader = new Shader(SHADERDIR "TexturedVertex.glsl",	SHADERDIR "processfrag.glsl");
	if (!processShader->LinkProgram() || !sceneShader->LinkProgram() || !reflectShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram()) return;		quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"lava.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetTexture2(SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"icyhell_ft.jpg", TEXTUREDIR"icyhell_bk.jpg",
		TEXTUREDIR"icyhell_up.jpg", TEXTUREDIR"icyhell_dn.jpg",
		TEXTUREDIR"icyhell_rt.jpg", TEXTUREDIR"icyhell_lf.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);

	if (!cubeMap || !quad->GetTexture() || !heightMap->GetTexture() || !heightMap->GetBumpMap()) return;

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(quad->GetTexture2(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);	init = true;
	waterRotate = 0.0f;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f * 20, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	
	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	glGenFramebuffers(1, &bufferFBO); 
	glGenFramebuffers(1, &processFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;
	delete sceneShader;
	delete processShader;
	currentShader = 0;
	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);

}

void Renderer::PressKeyFunction()
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_M)) Patricalmove = true;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_N)) Patricalmove = false;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) Autocameraon = true;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) Autocameraon = false;
	
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_P))	blurval = 0.1;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_O)) blurval = 1.0;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_V)) {
		changewor = true;
		PlaySound(TEXT("wildthings.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_C)) {
		changewor = false;
		PlaySound(TEXT("Disturbed.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
}

void Renderer::UpdateScene(float msec) {
	if (msec > 1000.0f) return;
	
	if(Autocameraon == true) camera->AutoCamera(msec);
	
	if(Autocameraon == false) camera->UpdateCamera(msec);

	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 4000.0f;		int i = 1;	if (changewor == true)	{		if (timePassed < 0) {			timePassed = 0;		}		timePassed += msec / 1000;		texchange = timePassed / totaltimechange;		if (texchange > 1)			texchange = 1.0f;	}	else 	{		if (timePassed > 10) timePassed = 10;		if (msec > 1000.0f) return;		timePassed -= msec / 1000; 		texchange = timePassed / totaltimechange;		if (timePassed < 0) timePassed = 0.0f;		if (texchange < 0) texchange = 0.0f;	}	PressKeyFunction();}
void Renderer::RenderScene() {

glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
projMatrix = Matrix4::Perspective(1.0f, 15000.0f * 20, (float)width / (float)height, 45.0f);
if (Patricalmove == true) MakePartMove(1);

DrawSkybox();
DrawHeightmap();
DrawWater();
DrawParticles();

DrawScene();
glBindFramebuffer(GL_FRAMEBUFFER, 0);
DrawPostProcess();
PresentScene();

SwapBuffers();}void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), blurval / width, blurval / height);	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"isVertical"), 0);
		postQuad->SetTexture(bufferColourTex[0]);
		postQuad->Draw();
		// Now to swap the colour buffers , and do the second blur pass
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
		postQuad->SetTexture(bufferColourTex[1]);
		postQuad->Draw();	}	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);}void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	postQuad->SetTexture(bufferColourTex[0]);
	postQuad->Draw();
	glUseProgram(0);}void Renderer::DrawScene() {
	
	SetCurrentShader(sceneShader);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	heightMap->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);	}

void Renderer::combineBuffers() {
	SetCurrentShader(sceneShader);
}void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}void Renderer::DrawParticles(){	SetCurrentShader(ParticleShader);
	glUseProgram(currentShader->GetProgram());
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "projMatrix"), 1, false, (float*)& projMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "viewMatrix"), 1, false, (float*)& viewMatrix);

	int widthmapx = (HEIGHTMAP_X / 2) * (1000 / 10) * 10; //16.0f * 20
	Vector3 tempposition = Vector3(0, 4000, 0);

	for (int l = 0; l < widthmapx; l += widthmapx / 20)
	{
		for (int j = 0; j < widthmapx; j += widthmapx / 20)
		{
			for (int i = 0; i < widthmapx; i += widthmapx / 20)
			{
				modelMatrix = Matrix4::Translation(tempposition) * Matrix4::Scale(Vector3(100, 100, 100));
				glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)& modelMatrix);
				Particles->Draw();
				tempposition.x += widthmapx / 20;
			}
			tempposition.x = 0;
			tempposition.z += widthmapx / 20;
		}
		tempposition.z = 0;
		tempposition.y += widthmapx / 20;
	}

	tempposition = Vector3(0, 40000, 0);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f * 20, (float)width / (float)height, 45.0f);}void Renderer::DrawHeightmap() {
	SetCurrentShader(lightShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();
	heightMap->Draw();

	glUseProgram(0);

}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),"cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"cubeTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"diffuseTex2"), 6);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(),"changetex"), texchange);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 150 * HEIGHTMAP_Y / 3.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) * Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 100.0f));

	UpdateShaderMatrices();
	quad->Draw();
	glUseProgram(0);
}

void Renderer::MakePartMove(int n)
{
	Particles->ParticalsMove(n);
}


