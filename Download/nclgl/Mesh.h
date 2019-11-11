#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER , COLOUR_BUFFER , TEXTURE_BUFFER, NORMAL_BUFFER, INDEX_BUFFER, MAX_BUFFER
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
	


protected:
	void BufferData();
	void GenerateNormals();
	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;


	GLuint numIndices;
	unsigned int* indices;



	Vector3* vertices;
	Vector4* colours;
	Vector3* normals;
	GLuint texture;
	Vector2 * textureCoords;
};

