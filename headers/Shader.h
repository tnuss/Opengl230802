#pragma once

#include <string>
#include <glad/glad.h>
//#include "transform.h"

class Shader
{
public:
	Shader(const std::string& fileName);
	Shader(const std::string& fileName, const std::string& fileName2);

	void AttachValidate();
	void Bind();
	//void Update(const Transform& transform, const Camera& camera);

	virtual ~Shader();

	//std::string vsUnfrmXSize = "vsXSize";
	//GLuint vsUnfrmXSizeLoc = -1;
	//std::string vsUnfrmYSize = "vsYSize";
	//GLuint vsUnfrmYSizeLoc = -1;
	//std::string vsUnfmXDimScale = "vsXDimScale";
	//GLuint vsUnfmXDimScaleLoc = -1;
	//GLuint gTranslationLocation = -1;
		//uniform mat4 mv_matrix;
		//uniform mat4 mv_matrix;
	//GLuint vsMVLoc;
	//GLuint vsProjLoc;
	//GLuint vsVLoc;

	//std::string unfrmTexUnit = "unfrmTexUnit";
	//GLuint unfrmTexUnitLoc;

	inline GLuint GetShaderProgram() 
	{
		return m_program;
	}

protected:
private:
	static const unsigned int NUM_SHADERS = 2;
	static const unsigned int NUM_UNIFORMS = 0;
	void operator=(const Shader& shader) {};
	Shader(const Shader& shader) {};

	std::string LoadShader(const std::string& fileName);
	void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	GLuint CreateShader(const std::string& text, unsigned int type);

	GLuint m_program = 0;
	GLuint m_shaders[NUM_SHADERS];
	//GLuint m_uniforms[NUM_UNIFORMS];

};
