#include "Mesh.h"

#include <cmath>
#include <random>

void Mesh::testcolours() {
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	Vector4* A = (Vector4*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	bool v = true;

	A[0].x = rand() / (float)RAND_MAX;
	
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Mesh::ParticalsMove(int n) {
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	Vector3* A = (Vector3*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	bool v = true;

	for (int i = 0; i < 100; i++)
	{
		if (n == 8) {
			A[i].x *= 1.01f;
			A[i].y *= 1.01f;
			A[i].z *= 1.01f;
		}
		else if (n == 2)
		{
			A[i].x /= 1.01f;
			A[i].y /= 1.01f;
			A[i].z /= 1.01f;
		}
		else if (n == 1)
		{
			
			A[i].y -= 0.1f;
			
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

Mesh::Mesh(void)
{
	for (int i = 0; i < MAX_BUFFER; ++i)
	{
		bufferObject[i] = 0;
	}
	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	vertices = NULL;
	colours = NULL;
	type = GL_TRIANGLES;
	texture = 0;
	textureCoords = NULL;
	normals = NULL;
	tangents = NULL;
	bumpTexture = 0;
	indices = NULL;
	numIndices = 0;


}

Mesh::~Mesh(void)
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	delete [] vertices;
	delete [] colours;
	delete[] normals;
	delete[] tangents;
	glDeleteTextures(1, &bumpTexture);
	glDeleteTextures(1, &texture);
	delete[] textureCoords;
	delete[] indices;

}


Mesh* Mesh::Particles() {

	Mesh* n = new Mesh();
	n->numVertices = 100;
	n->vertices = new Vector3[n->numVertices];
	
	std::default_random_engine generator;
	std::uniform_real_distribution<double> u1Distribution(-1.0, 1.0);
	std::uniform_real_distribution<double> u2Distribution(0.0, 1.0);
	std::uniform_real_distribution<double> u3Distribution(0.0, 10.0);
	int f = u3Distribution(generator);
	for (int i = 0; i < n->numVertices; i++)
	{
		//float x = (float)(((((cos(90 - rand() % 180))*sin(180 - rand() % 360)))));
		//float y = (float)(sin((((cos(90 - rand() % 180))*cos(180 - rand() % 360)))));
		//float z = (float)(sin(((sin(90 - rand() % 180)))));
		////float z = 0.5f + (float)(rand() % 129) / 128.0f;

		/*
		float u1 = randomRange( -1.0f, 1.0f );
		float u2 = randomRange( 0.0f, 1.0f );
		float r = sqrt( 1.0f - u1*u1 );
		float theta = 2.0f*math::pi<float>()*u2;

		return Vector3( r*cos( theta ), r*sin( theta ), u1 );
		
		*/

		float u1 = u1Distribution(generator);
		float u2 = u2Distribution(generator);
		float r = sqrt(1.0f - u1 * u1);
		float theta = 2.0f * 3.14159f * u2;

		n->vertices[i] = Vector3(r * cos(theta) * f, r * sin(theta) * f, u1 * f);
	}
	

	n->colours = new Vector4[n->numVertices];


	for (int i = 0; i < n->numVertices; i++)
	{
		float x = 1.0f;
		float y = 0.0f;
		float z = 0.0f;
		n->colours[i] = Vector4(x, y, z, 0.2f);
	}

	n->type = GL_POINTS;
	n->BufferData();

	return n;
}
Mesh* Mesh::GenerateTriangle() {
	Mesh* m = new Mesh();
	m->numVertices = 3;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.5f, 0.0f);
	m->textureCoords[1] = Vector2(1.0f, 1.0f);
	m->textureCoords[2] = Vector2(0.0f, 1.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->newcolours = new Vector4[m->numVertices];
	m->newcolours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->newcolours[1] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->newcolours[2] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	m->BufferData();

	return m;

}




void Mesh::GenerateNormals() {
	if (!normals) {
		normals = new Vector3[numVertices];

	}
	for (GLuint i = 0; i < numVertices; ++i) {
		normals[i] = Vector3();

	}
	if (indices) { // Generate per - vertex normals
		for (GLuint i = 0; i < numIndices; i += 3) {
			unsigned int a = indices[i];
			unsigned int b = indices[i + 1];
			unsigned int c = indices[i + 2];

			Vector3 normal = Vector3::Cross( 
				(vertices[b] - vertices[a]), (vertices[c] - vertices[a]));

			normals[a] += normal;
			normals[b] += normal;
			normals[c] += normal;

		}

	}
	else { // It ’s just a list of triangles , so generate face normals
		for (GLuint i = 0; i < numVertices; i += 3) {
			Vector3& a = vertices[i];
			Vector3& b = vertices[i + 1];
			Vector3& c = vertices[i + 2];

			Vector3 normal = Vector3::Cross(b - a, c - a);

			normals[i] = normal;
			normals[i + 1] = normal;
			normals[i + 2] = normal;

		}

	}

	for (GLuint i = 0; i < numVertices; ++i) {
		normals[i].Normalise();

	}

}



Mesh* Mesh::GenerateQuad() {
	Mesh* m = new Mesh();
	
	 m -> numVertices = 4;
	m -> type = GL_TRIANGLE_STRIP;
	
	 m -> vertices = new Vector3[m -> numVertices];
	m -> textureCoords = new Vector2[m -> numVertices];
	m -> colours = new Vector4[m -> numVertices];
	 m -> normals = new Vector3[m -> numVertices];
	 m -> tangents = new Vector3[m -> numVertices];
	
	 m -> vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
	 m -> vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
	 m -> vertices[2] = Vector3(1.0f, -1.0f, 0.0f);
	 m -> vertices[3] = Vector3(1.0f, 1.0f, 0.0f);
	
	 m -> textureCoords[0] = Vector2(0.0f, 1.0f);
	 m -> textureCoords[1] = Vector2(0.0f, 0.0f);
	 m -> textureCoords[2] = Vector2(1.0f, 1.0f);
	 m -> textureCoords[3] = Vector2(1.0f, 0.0f);
	
		 for (int i = 0; i < 4; ++i) {
		 m -> colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		 m -> normals[i] = Vector3(0.0f, 0.0f, -1.0f);
		 m -> tangents[i] = Vector3(1.0f, 0.0f, 0.0f);
		
	}
	
		 m -> BufferData();
	return m;

}



void Mesh::BufferData() {
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3),
		vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	if (textureCoords) { // This bit is new !
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2),
			textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);

	}
	if (colours) {
		if (bufferObject[COLOUR_BUFFER] == 0) {
			glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);

			glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);

			glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4),
				colours, GL_STATIC_DRAW);
			glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(COLOUR_BUFFER);
		}
		else 
		{
			Vector4* A = (Vector4*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			bool v = true;
		}
	}


	if (indices) {
		 glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
			 bufferObject[INDEX_BUFFER]);
		 glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint),
			 indices, GL_STATIC_DRAW);
		
	}
	if (normals) {
		 glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		 glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		 glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3),
		 normals, GL_STATIC_DRAW);
		 glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		 glEnableVertexAttribArray(NORMAL_BUFFER);
		
	}

	if (tangents) {
		glGenBuffers(1, &bufferObject[TANGENT_BUFFER]);
		 glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		 glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), tangents, GL_STATIC_DRAW);
		 glVertexAttribPointer(TANGENT_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		 glEnableVertexAttribArray(TANGENT_BUFFER);
			}


	glBindVertexArray(0);

}


void Mesh::Draw() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glActiveTexture(GL_TEXTURE1); 
	glBindTexture(GL_TEXTURE_2D, bumpTexture);

	glBindVertexArray(arrayObject);
	 if (bufferObject[INDEX_BUFFER]) {
		 glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
		
	}
	 else {
		 glDrawArrays(type, 0, numVertices);
		
	}
	 glBindVertexArray(0);
	 glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::GenerateTangents() {
	 if (!tangents) {
		 tangents = new Vector3[numVertices];
		
	}
	 if (!textureCoords) {
		 return; // Can ’t use tex coords if there aren ’t any !
		
	}
	 for (GLuint i = 0; i < numVertices; ++i) {
		 tangents[i] = Vector3();
		
	}
	
		 if (indices) {
		 for (GLuint i = 0; i < numIndices; i += 3) {
			 int a = indices[i];
			 int b = indices[i + 1];
			 int c = indices[i + 2];
			
				 Vector3 tangent = GenerateTangent(vertices[a], vertices[b],
					 vertices[c], textureCoords[a],
					 textureCoords[b], textureCoords[c]);
			
			 tangents[a] += tangent;
			 tangents[b] += tangent;
			 tangents[c] += tangent;
			
		}
		
	}
	 else {
		 for (GLuint i = 0; i < numVertices; i += 3) {
			 Vector3 tangent = GenerateTangent(vertices[i], vertices[i + 1],
				 vertices[i + 2], textureCoords[i],
				 textureCoords[i + 1], textureCoords[i + 2]);
			
			 tangents[i] += tangent;
			tangents[i + 1] += tangent;
			tangents[i + 2] += tangent;
			
		}
		
	}
	 for (GLuint i = 0; i < numVertices; ++i) {
		 tangents[i].Normalise();
		
	}
	}Vector3 Mesh::GenerateTangent(const Vector3& a, const Vector3& b,
	const Vector3 & c, const Vector2& ta,
	const Vector2 & tb, const Vector2& tc) {
	Vector2 coord1 = tb - ta;
	Vector2 coord2 = tc - ta;
	
	 Vector3 vertex1 = b - a;
	Vector3 vertex2 = c - a;
	
	 Vector3 axis = Vector3(vertex1 * coord2.y - vertex2 * coord1.y);
	
	 float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);
	
	 return axis * factor;
	}