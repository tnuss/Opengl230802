#include<iostream>
#include<string>
#include<vector>
#include"windows.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"
//#include<stb/stb_image.h>

int CMDLineNum = 5;

const int numVAOs = 1;
const int numVBOs = 3;
const int numEBOs = 1;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint ebo[numEBOs];

GLuint vsShapeSwitchLoc = 0;
GLuint vsShapeSwitch = 0;
//glUniform1ui(vsShapeSwitchLoc, vsShapeSwitch);

float radVar = 0.1f;
GLuint rotateLoc = 0;
glm::mat4 m4Rotate;
GLint scaleLoc = 0;
glm::mat4 m4Scale;
GLint vsMVLoc = 0;
glm::mat4 m4MV;
GLint vsProjectionLoc;
glm::mat4 m4Projection;
glm::mat4 translt;

std::vector<Texture*> vecTextures;

//--------- Funct Declarations

int DoCmdLine(int, char* []);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

GLFWwindow* InitGladAndWindow(int width, int height);
int main(int argc, char* argv[]);
int gWinWidth, gWinHeight;
float gWinAspect = 0.0f;

int DoRect(GLuint vvbo, GLuint eebo, Shader& sshader);   // 1
int DoCube(GLuint vvbo, GLuint eebo, Shader& sshader); // 3
int DoRectTexture(GLuint vvbo, GLuint eebo, Shader& sshader); // 2
int DoAxes(GLuint vvbo, Shader& sshader); // 4
int DoCubePerspect(GLuint vvbo, Shader& sshader); // 5

//enum class VTypes
//{
//    POSITION_VB,
//    COLOR_VB,
//    TEXCOORD_VB,
//    NORMAL_VB,
//    INDEX_VB
//};
//std::vector <VTypes> PositionV = { VTypes::POSITION_VB };

int main(int argc, char* argv[])
{
    DoCmdLine(argc, argv);

    std::cout << "CMDLineNum: " << CMDLineNum << '\n';
    glfwInit();

    GLFWwindow* window = InitGladAndWindow(800, 800);
    if (window == NULL)
        return -1;

    std::string shaderDirName = "resources/";
    std::string vertFileName = shaderDirName;
    std::string fragFileName = shaderDirName;

    switch (CMDLineNum)
    {
        case 1:
        case 2:
            vertFileName += "shader";
            fragFileName += "shader";
            break;

        case 3:
            vertFileName += "Shdr-Cube";
            fragFileName += "Shdr-Cube";
            break;

        case 4:
            vertFileName += "XYZLines";
            fragFileName += "XYZLines";
            break;

        case 5:
            vertFileName += "CubePerspect";
            fragFileName += "CubePerspect";
            break;

        default:
            return 0;
            break;
    }

    Shader shader(vertFileName, fragFileName);
    shader.Bind();

    glGenVertexArrays(numVAOs, vao);
    glGenBuffers(numVBOs, vbo);
    glGenBuffers(numEBOs, ebo);

            // Load textures ... texUnit will assign 0, 1, 2 ... to texture for 
            // use with multiple textures
    unsigned int texUnit = 0;
    //Texture tex = Texture("resources/bricks.jpg", shader, texUnit);
    Texture tex("resources/brick.png", shader, texUnit);
    if (!tex.IsTextureLoaded())
        return -1;
    
      // NOTE!! EXAMPLE FOR USING VECTOR OF TEXTURES???
    vecTextures.push_back(&tex);
    Texture* rTex = NULL;
    rTex = vecTextures[vecTextures.size()-1];
    // NOTE!! FOR USING VECTOR OF TEXTURES???

    // clear then activate vao
    glBindVertexArray(0);
    glBindVertexArray(vao[0]);

    // *******  HERE BE WHERE VERTEX BUFFERS BE SET **************

    int numIndices = 0;
    int numVertices = 0;

    switch (CMDLineNum)
    {
    case 1:
       numIndices = DoRect(vbo[0], ebo[0], shader);
       if (numIndices < 1)
           return -1;
       break;

    case 2:
        numIndices = DoRectTexture(vbo[0], ebo[0], shader);
        if (numIndices < 1)
            return -1;
        break;

    case 3:
            // indices in this case is number of vertexes...
            // also rotates
        numVertices = DoCube(vbo[0], ebo[0], shader);
        if (numVertices < 1)
            return -1;
        break;

    case 4:
        // indices in this case is number of vertexes...
        // also rotates
        numVertices = DoAxes(vbo[0], shader);
        if (numVertices < 1)
            return -1;
        break;

    case 5:
        numVertices = DoCubePerspect(vbo[0], shader);
        if (numVertices < 1)
            return -1;
        break;

    default:
        break;
    }

    // Specify the color of the background
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
   // glEnable(GL_DEPTH_TEST);

    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime = 0;
    float cutoffTime = 150.0f;
    float zOffset = -5.0;

    switch (CMDLineNum)
    {
    case 4:
        radVar = 0.15f;
        cutoffTime = 800.0f;
        break;

    default:
        radVar = 0.15f;
        cutoffTime = 150.0f;
        break;
    }

    // get ticks per second --- uses windows.h
    QueryPerformanceFrequency(&frequency);
        // start timer
    QueryPerformanceCounter(&t1);
    // 'compare to' timer
    QueryPerformanceCounter(&t2);

	while (!glfwWindowShouldClose(window))
	{
        processInput(window);

        // in millisecs
        elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
            // greater than 50 ms reset
        if (elapsedTime > cutoffTime)
        {
           radVar += 0.15f;
           if (radVar > 6.28f)
                radVar = 0.15f;

            std::cout << elapsedTime << " ms.\n";
            std::cout << radVar << " radians\n";
            QueryPerformanceCounter(&t1);
        }
        QueryPerformanceCounter(&t2);
        
        // Clean the back buffer and assign the new color to it
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        if (CMDLineNum == 3 || 5)
        {
            // 
            glFrontFace(GL_CCW);
            //glFrontFace(GL_CW);
            glCullFace(GL_BACK);
        }
             // MAY NEED TO BIND WHICH VAO IF HAVE MULTIPLE ONES
        //glBindVertexArray(vao[0]);
             // MAY NEED TO BIND WHICH VAO IF HAVE MULTIPLE ONES
             
            // --- need to keep track of which texture unit that is being used??????
                // WHY DON"T WE NEED THIS BECAUSE ONLY ONE TEXTURE????? MAYBE?????
                // if 2+ textures then need to activate and bind each before drawing
        //rTex->Bind();
        //tex.Bind();
                // WHY DON"T WE NEED THIS BECAUSE ONLY ONE TEXTURE????? MAYBE?????
        //glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "recvTexture"), 0);
       // tex.Bind();
       //glBindVertexArray(vao[0]);

                 // ABSTRACT THIS OUT  by switch??

        if (CMDLineNum == 3 || CMDLineNum == 4)
        {
            m4Scale = glm::mat4(1.0f);
            m4MV = glm::mat4(1.0f);
            m4Scale = glm::rotate(m4Scale, radVar, glm::vec3(0.0f, 1.0f, 1.0f));
            //m4MV = glm::translate(m4MV, glm::vec3(0.25f, 0.25f, 0.25f));
            //m4MV = glm::rotate(m4MV, radVar, glm::vec3(1.0f, 0.5f, 0.0f));
            m4MV = glm::rotate(m4MV, radVar, glm::vec3(1.0f, 1.0f, 1.0f));
            glUniformMatrix4fv(scaleLoc, 1, GL_FALSE, glm::value_ptr(m4Scale));
            glUniformMatrix4fv(vsMVLoc, 1, GL_FALSE, glm::value_ptr(m4MV));

            m4Projection = glm::perspective(glm::radians(45.0f), gWinAspect, 0.1f, 100.0f);
            //glUniformMatrix4fv(vsProjectionLoc, 1, GL_FALSE, glm::value_ptr(m4Projection));
        }
        if (CMDLineNum == 5)
        {
            zOffset += -.01f;
            //m4MV = glm::translate(m4MV, glm::vec3(0.0f, 0.0f, -0.1));
            m4MV = glm::translate(glm::mat4(1), glm::vec3(1.0f, 1.0f, -7.0));

            m4MV = glm::rotate(m4MV, radVar, glm::vec3(0.0f, 1.0f, 1.0f));
            //std::cout << "radVar Change of Z: " << m4MV[3].z << '\n';
            m4Projection = glm::perspective(glm::radians(45.0f), gWinAspect, 0.1f, 500.0f);
            glUniformMatrix4fv(vsProjectionLoc, 1, GL_FALSE, glm::value_ptr(m4Projection));
            glUniformMatrix4fv(vsMVLoc, 1, GL_FALSE, glm::value_ptr(m4MV));
            //glUniformMatrix4fv(vsProjLoc, 1, GL_FALSE, glm::value_ptr(m4Projection));
        }
                 // ABSTRACT THIS OUT

        switch (CMDLineNum)
        {
            // pos and color as a uniform
        case 1:
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
            break;
            // pos, texture
        case 2:
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
            break;
            // cube 
        case 3:
        case 5:
            glDrawArrays(GL_TRIANGLES, 0, numVertices);
            break;

        case 4:
            glDrawArrays(GL_LINES, 0, 2);
            glDrawArrays(GL_LINES, 2, 4);
            glDrawArrays(GL_LINES, 4, 6);
            break;

        default:
            break;
        }
        // Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}
    
    //glDeleteTextures(1,tex???);
    glDeleteBuffers(1, ebo);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, vao);

    tex.~Texture();

	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

    return 0;
 }

//-----------------------------------------
int DoCubePerspect(GLuint vbo, Shader& shader)
{
    float vertsCube[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    int numVerts = sizeof(vertsCube) / (sizeof(float) * 5);

    //std::cout << "Num Verts: " << sizeof(vertsCube)/ (sizeof(float) * 5) << '\n';

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertsCube), vertsCube, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // single color #2 layout var
    glVertexAttrib4f(2, 0.86f, 0.9f, 0.32f, 1.0f);

    // ABSTRACT THIS OUT
    m4Scale = glm::mat4(1.0f);
    m4MV = glm::mat4(1.0f);
           // move it back into the distance 
    m4MV = glm::translate(m4MV, glm::vec3(0.0f, 0.0f, -7.0f));
    //m4Scale = glm::scale(m4Scale, glm::vec3(0.2, 0.2, 0.2));

    m4MV = m4Scale * m4MV;
           // model/view
    //GLint vsMVLoc = 0;
    //vsMVLoc = 0;
    vsMVLoc = glGetUniformLocation(shader.GetShaderProgram(), "vsMVMatrix");
    if (vsMVLoc < 0)
    {
        std::cout << "Shader vsMVMatrix uniform Lookup Error: " << vsMVLoc << '\n';
        return -1;
    }

    vsProjectionLoc = glGetUniformLocation(shader.GetShaderProgram(), "vsProjMatrix");
    if (vsProjectionLoc < 0)
    {
        std::cout << "Shader vsProjMatrix uniform Lookup Error: " << vsProjectionLoc << '\n';
        return -1;
    }
    //m4Projection = glm::perspective(glm::radians(45.0f), gWinAspect, 0.01f, 1000.0f);
    //glUniformMatrix4fv(vsProjectionLoc, 1, GL_FALSE, glm::value_ptr(m4Projection));
    // rotate
    //radVar = 0.1f;
    //m4Scale = glm::rotate(m4Scale, radVar, glm::vec3(0.0f, 1.0f, 1.0f));
    //glUniformMatrix4fv(scaleLoc, 1, GL_FALSE, glm::value_ptr(m4Scale));

    return numVerts;
}

//----------------------------------------
int DoAxes(GLuint vbo, Shader& shader)
{
    GLfloat vertices[] =
    { //    COORDINATES                  /     COLORS           //
        -1.0f,  0.0f,  0.0f,   0.9f, 0.3f, 0.6f, 1.0f, // Lower left
         1.0f,  0.0f,  0.0f,   0.9f, 0.3f, 0.6f, 1.0f, // Lower r
         0.0f,  1.0f,  0.0f,   0.3f, 0.9f, 0.6f, 1.0f,  // 
         0.0f, -1.0f,  0.0f,   0.3f, 0.9f, 0.6f, 1.0f,  // 

            // NEED PERSPECTIVE/ORTHO to VIEW Z LINE????
         0.0f,  0.0f, -1.0f,   0.3f, 0.6f, 0.9f, 1.0f,  //
         0.0f,  0.0f,  1.0f,   0.3f, 0.6f, 0.9f, 1.0f  // 
    };

    int numVertices = 6;
    //doLineStrip = true;
    //doLineLoop = false;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
    glEnableVertexAttribArray(0);
    // color coord attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

        // single color #1 layout var
   // glVertexAttrib4f(1, 0.8f, 0.8f, 0.32f, 1.0f); // yellow
    //glVertexAttrib4f(1, 0.8f, 0.1f, 0.32f, 1.0f);  // red

    vsMVLoc = -1;
    //shMVLoc = glGetUniformLocation(shader.GetShaderProgram(), "scale_matrix");
    vsMVLoc = glGetUniformLocation(shader.GetShaderProgram(), "vs_mvmatrix");
    if (vsMVLoc < 0)
    {
        std::cout << "Shader vs_mvmatrix uniform Lookup Error: " << vsMVLoc << '\n';
        return -1;
    }

    vsProjectionLoc = 0;
    //vsProjectionLoc = glGetUniformLocation(shader.GetShaderProgram(), "vs_projection");
    //if (vsProjectionLoc < 0)
    //{
    //    std::cout << "Shader vs_projection uniform Lookup Error: " << vsProjectionLoc << '\n';
    //    return -1;
    //}
    //m4Projection = glm::perspective(glm::radians(45.0f), gWinAspect, 0.1f, 100.0f);
    //glUniformMatrix4fv(vsProjectionLoc, 1, GL_FALSE, glm::value_ptr(m4Projection));

    return numVertices;
}

//-----------------------------------------
int DoCube(GLuint vbo, GLuint ebo, Shader& shader)
{
       // THIS ROTATES AS WELL
       // ====================

    float vertsCube[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    int numVerts = sizeof(vertsCube) / (sizeof(float) * 5);

    //std::cout << "Num Verts: " << sizeof(vertsCube)/ (sizeof(float) * 5) << '\n';

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertsCube), vertsCube, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

        // texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // single color #2 layout var
    glVertexAttrib4f(2, 0.86f, 0.9f, 0.32f, 1.0f);

                 // ABSTRACT THIS OUT
    m4Scale = glm::mat4(1.0f);
    m4Scale = glm::scale(m4Scale, glm::vec3(0.75, 0.75, 0.75));
    scaleLoc = 0;
    scaleLoc = glGetUniformLocation(shader.GetShaderProgram(), "scale_matrix");
    // = glGetUniformLocation(shader.GetShaderProgram(), "sc_matrix");
    if (scaleLoc < 0)
    {
        std::cout << "Shader scale_matrix uniform Lookup Error: " << scaleLoc << '\n';
        return -1;
    }
    // rotate
    radVar = 0.1f;
    m4Scale = glm::rotate(m4Scale, radVar, glm::vec3(0.0f, 1.0f, 1.0f));

    glUniformMatrix4fv(scaleLoc, 1, GL_FALSE, glm::value_ptr(m4Scale));

    //glm::mat4 rotMat = glm::rotate(mScale, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));


        // return #vertices
    return numVerts;;
}

//------------------------------------------------
int DoRectTexture(GLuint vbo, GLuint ebo, Shader& shader)
{

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };

    int numElementIndices = 6;
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

      // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // single color #2 layout var
    glVertexAttrib4f(1, 0.86f, 0.9f, 0.32f, 1.0f);

    vsShapeSwitchLoc = -1;
    vsShapeSwitchLoc = glGetUniformLocation(shader.GetShaderProgram(), "vsShapeSwitch");
    // = glGetUniformLocation(shader.GetShaderProgram(), "sc_matrix");
    if (vsShapeSwitchLoc < 0)
    {
        std::cout << "Shader vsShapeSwitch uniform Lookup Error: " << vsShapeSwitchLoc << '\n';
        return -1;
    }

    // cmdline value...
    vsShapeSwitch = 2;
    glUniform1ui(vsShapeSwitchLoc, vsShapeSwitch);

    //// texture coord attribute
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    //glEnableVertexAttribArray(2);

    // --- need to keep track of which texture unit that is being used??????
    //tex.Bind(0);
    //glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "recvTexture"), 0);

    return numElementIndices; // element count
    // color attribute
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    //// texture coord attribute
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    //glEnableVertexAttribArray(2);
}

//---------------------------------------
int DoRect(GLuint vbo, GLuint ebo, Shader& shader)
{
    float vertices[] = {
        // positions          // colors           // texture coords
        // 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        // 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        //-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        //-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // single color #1 layout var
    glVertexAttrib4f(1, 0.1f, 0.7f, 0.1f, 1.0f);
    
    vsShapeSwitchLoc = 0;

    vsShapeSwitchLoc = glGetUniformLocation(shader.GetShaderProgram(), "vsShapeSwitch");
    // = glGetUniformLocation(shader.GetShaderProgram(), "sc_matrix");
    if (vsShapeSwitchLoc < 0)
    {
        std::cout << "Shader vsShapeSwitch uniform Lookup Error: " << vsShapeSwitchLoc << '\n';
        return -1;
    }

        // cmdline value...
    vsShapeSwitch = 1;
    glUniform1ui(vsShapeSwitchLoc, vsShapeSwitch);

    return 6; // element count
    // color attribute
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    //// texture coord attribute
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    //glEnableVertexAttribArray(2);
}
//--------------------------------------------------
GLFWwindow* InitGladAndWindow(int windx = 800, int windy = 800)
{
    //windx = 800;
    //windy = 800;
    //
     //	// Tell GLFW what version of OpenGL we are using 
     //	// In this case we are using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    // Tell GLFW we are using the CORE profile
    // So that means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    //	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
    GLFWwindow* window = glfwCreateWindow(windx, windy, "YoutubeOpenGL", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
        
        // set global vars
    glfwGetWindowSize(window, &gWinWidth, &gWinHeight);
    gWinAspect = round((float)(gWinWidth / (float)gWinHeight) * 10000) / 10000;

    //	//Load GLAD so it configures OpenGL
    gladLoadGL();

    // set default
    // doesn't work with 2D Lines????
//glEnable(GL_DEPTH_TEST);

//std::cout << "Max Vertex Attributes: " << GL_MAX_VERTEX_ATTRIBS << '\n';

//	// Specify the viewport of OpenGL in the Window
//	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
    glViewport(0, 0, windx, windy);

    glClearColor(0.17f, 0.15f, 0.27f, 1.0f);

    return window;
}

//-----------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

//---------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    gWinWidth = width;
    gWinHeight = height;
    gWinAspect = round((float)(gWinWidth / (float)gWinHeight) * 10000) / 10000;


}

//------------------------------------------
int DoCmdLine(int argc, char* argv[])
{
    std::cout << "Opengl Lines" << '\n';
    std::cout << "CMD #Args: " << argc << '\n';

    int ArgvNum = 0;

    for (int i = 0; i < argc; i++)
    {
        std::cout << i << ": " << argv[i] << '\n';

        // blank or 0, uniform var for line color
        // 1 texture for line color
        if (i == 1)
        {
            std::string cmd(argv[i]);
            if (cmd == "1" || "2" || "3" || "4" || "5")
            {

                ArgvNum = std::stoi(cmd);
                std::cout << " Cmd Line -> " << ArgvNum << '\n';
            }
            else
                ArgvNum = 0;

            CMDLineNum = ArgvNum;
        }

    }

    return 0;
}

//=====================================================

//// Vertices coordinates
//GLfloat vertices[] =
//{ //               COORDINATES                  /     COLORS           //
//	-0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower left corner
//	 0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower right corner
//	 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f, // Upper corner
//	-0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner left
//	 0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner right
//	 0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f  // Inner down
//};
//
//// Indices for vertices order
//GLuint indices[] =
//{
//	0, 3, 5, // Lower left triangle
//	3, 2, 4, // Lower right triangle
//	5, 4, 1 // Upper triangle
//};

// Vertices coordinates
//GLfloat vertices[] =
//{ //     COORDINATES     /        COLORS      /   TexCoord  //
//	-0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
//	-0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
//	 0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
//	 0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
//};
//
//// Indices for vertices order
//GLuint indices[] =
//{
//	0, 2, 1, // Upper triangle
//	0, 3, 2 // Lower triangle
//};
//
//
//int main()
//{
//	// Initialize GLFW
//	glfwInit();
//
//	// Tell GLFW what version of OpenGL we are using 
//	// In this case we are using OpenGL 3.3
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	// Tell GLFW we are using the CORE profile
//	// So that means we only have the modern functions
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
//	GLFWwindow* window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL);
//	// Error check if the window fails to create
//	if (window == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	// Introduce the window into the current context
//	glfwMakeContextCurrent(window);
//
//	//Load GLAD so it configures OpenGL
//	gladLoadGL();
//	// Specify the viewport of OpenGL in the Window
//	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
//	glViewport(0, 0, 800, 800);
//
//	// Generates Shader object using shaders defualt.vert and default.frag
//	//Shader shaderProgram("Resources/default.vert", "Resources/default.frag");
//	Shader shaderProgram = Shader("Resources/default.vert", "Resources/default.frag");
//
//	// Generates Vertex Array Object and binds it
//	VAO VAO1;
//	VAO1.Bind();
//
//	// Generates Vertex Buffer Object and links it to vertices
//	VBO VBO1(vertices, sizeof(vertices));
//	// Generates Element Buffer Object and links it to indices
//	EBO EBO1(indices, sizeof(indices));
//
//	// Links VBO to VAO
//	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
//	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3*sizeof(float)));
//	//VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//	// Unbind all to prevent accidentally modifying them
//	VAO1.Unbind();
//	VBO1.Unbind();
//	EBO1.Unbind();
//			//define uniform var from shader for use here
//	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");
//
//		// texture
//	int widthImg, heightImg, numColCh;
//	unsigned char* bytes = stbi_load("Resources/pop_cat.png", &widthImg, &heightImg, &numColCh, 0);
//
//	GLuint texture;
//	glGenTextures(1, &texture); 
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, texture);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
//	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
//	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
//
//	// Assigns the image to the OpenGL Texture object
//	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
//	// Generates MipMaps
//	glGenerateMipmap(texType);
//
//	// Deletes the image data as it is already in the OpenGL Texture object
//	stbi_image_free(bytes);
//
//	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
//	glBindTexture(texType, 0);
//
//	GLuint tex0Uni = glad_glGetUniformLocation(shaderProgram.ID, "tex0");
//	shaderProgram.Activate();
//	glUniform1i(tex0Uni, 0);
//
//	// Main while loop
//	while (!glfwWindowShouldClose(window))
//	{
//		// Specify the color of the background
//		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
//		// Clean the back buffer and assign the new color to it
//		glClear(GL_COLOR_BUFFER_BIT);
//		// Tell OpenGL which Shader Program we want to use
//		shaderProgram.Activate();
//			// set scale in shader program
//		glUniform1f(uniID, 0.5f);
//		glBindTexture(GL_TEXTURE_2D, texture);
//
//		// Bind the VAO so OpenGL knows to use it
//		VAO1.Bind();
//		// Draw primitives, number of indices, datatype of indices, index of indices
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//		// Swap the back buffer with the front buffer
//		glfwSwapBuffers(window);
//		// Take care of all GLFW events
//		glfwPollEvents();
//	}
//
//
//
//	// Delete all the objects we've created
//	VAO1.Delete();
//	VBO1.Delete();
//	EBO1.Delete();
//	glDeleteTextures(1,&texture);
//	shaderProgram.Delete();
//
//	// Delete window before ending the program
//	glfwDestroyWindow(window);
//	// Terminate GLFW before ending the program
//	glfwTerminate();
//	return 0;
//}
