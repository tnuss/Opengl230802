#pragma once

//#include <GL/glew.h>
//#include<GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
//#include "obj_loader.h"

enum VertexTypes
{
	POSITION_VB,
	COLOR_VB,
	TEXCOORD_VB,
	NORMAL_VB,
	INDEX_VB
};

enum MeshDrawType
{
	USE_ARRAY,
	USE_INDICES
};

struct Vertex
{
public:
	Vertex(const glm::vec3& pos, const glm::vec2& texCoord, const glm::vec3& normal)
	{
		this->pos = pos;
		this->texCoord = texCoord;
		//this->normal = normal;
	}

	glm::vec3* GetPos() { return &pos; }
	glm::vec2* GetTexCoord() { return &texCoord; }
	//glm::vec3* GetNormal() { return &normal; }

private:
	glm::vec3 pos;
	glm::vec2 texCoord;
	//glm::vec3 normal;
};

class mesh
{
public:
	//mesh(const std::string& fileName);
	mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);

	inline void SetMeshDrawArray()
	{
		m_meshdrawtype = USE_ARRAY;
	};

	void SetMeshDrawElements()
	{
		m_meshdrawtype = USE_INDICES;
	};

	void Draw();

	virtual ~mesh();
protected:
private:
	static const unsigned int NUM_BUFFERS = sizeof(VertexTypes);
	void operator=(const mesh& mesh) {};
	mesh(const mesh& mesh) {};

	//void InitMesh(const IndexedModel& model);

	GLuint m_vertexArrayObject = 0;;
	GLuint m_vertexArrayBuffers[NUM_BUFFERS];

	MeshDrawType m_meshdrawtype = USE_ARRAY;

	unsigned int m_numIndices = 0;
	GLuint m_drawCount = 0;
};
