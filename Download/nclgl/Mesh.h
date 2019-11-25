#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER, TANGENT_BUFFER, INDEX_BUFFER, MAX_BUFFER
};

class Mesh
{
public:
	Mesh(void);
	~Mesh(void);

	static Mesh* GenerateQuad();
	void SetBumpMap(GLuint tex) { bumpTexture = tex; }
	GLuint GetBumpMap() { return bumpTexture; }
	void SetTexture(GLuint tex) { texture = tex; }
	void SetTexture2(GLuint tex) { texture2 = tex; }
	GLuint GetTexture() { return texture; }
	GLuint GetTexture2() { return texture2; }

	virtual void Draw();
	static Mesh* GenerateTriangle();
	void ParticalsMove(int n);
	void testcolours();
	static Mesh* Particles();
	static Mesh* ParticlesSky();

protected:
	void BufferData();
	void GenerateNormals();
	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;


	GLuint numIndices;
	unsigned int* indices;

	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3& a, const Vector3& b,
		const Vector3& c, const Vector2& ta,
		const Vector2& tb, const Vector2& tc);

	Vector3* tangents;
	GLuint bumpTexture;
	

	Vector3* vertices;
	Vector4* colours;
	Vector4* newcolours;
	Vector3* normals;

	GLuint texture;
	GLuint texture2;
	Vector2* textureCoords;
};

