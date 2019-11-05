#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER , COLOUR_BUFFER , TEXTURE_BUFFER , MAX_BUFFER
};

class Mesh
{
public:
	Mesh(void);
	~Mesh(void);

	static Mesh* GenerateQuad();

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() { return texture; }
	
	virtual void Draw();
	static Mesh* GenerateTriangle();
	enum MeshBuffer {
		 VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, MAX_BUFFER
	};

protected:
	void BufferData();

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;

	Vector3* vertices;
	Vector4* colours;

	GLuint texture;
	Vector2 * textureCoords;
};

