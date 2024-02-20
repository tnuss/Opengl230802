#include<iostream>
#include<string>
#include<vector>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"
//#include<stb/stb_image.h>

int CMDLineNum = 3;

const int numVAOs = 1;
const int numVBOs = 3;
const int numEBOs = 1;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint ebo[numEBOs];

//--------- Funct Declarations

int DoCmdLine(int, char* []);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

GLFWwindow* InitGladAndWindow(int width, int height);

int DoRect(GLuint vvbo, GLuint eebo);
int DoCube(GLuint vvbo, GLuint eebo);
int DoRectTexture(GLuint vvbo, GLuint eebo);

std::vector<Texture*> vecTextures;

//======= SO WHY DOES THE TEXTURE WORK NOW ==========
//         I JUST REARRAGED CODE A BIT ...
//            maybe since the texture was created locally in the DoRectTexture()
//===================================================

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

    switch (CMDLineNum)
    {
    case 1:
       numIndices = DoRect(vbo[0], ebo[0]);
       if (numIndices < 1)
           return -1;
       break;

    case 2:
        numIndices = DoRectTexture(vbo[0], ebo[0]);
        if (numIndices < 1)
            return -1;
        break;

    case 3:
            // indices in this case is number of vertexes...
        numIndices = DoCube(vbo[0], ebo[0]);
        if (numIndices < 1)
            return -1;
        break;

    default:
        break;
    }

    glm::mat4 mScale = glm::mat4(1.0f);
    mScale = glm::scale(mScale, glm::vec3(0.75, 0.75, 0.75));
    unsigned int scaleLoc = glGetUniformLocation(shader.GetShaderProgram(), "scale");
      // rotate
    mScale = glm::rotate(mScale, 1.0f, glm::vec3(0.0f, 1.0f, 1.0f));

    glUniformMatrix4fv(scaleLoc, 1, GL_FALSE, glm::value_ptr(mScale));

    //glm::mat4 rotMat = glm::rotate(mScale, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

    // Specify the color of the background
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
   // glEnable(GL_DEPTH_TEST);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
        processInput(window);

		// Clean the back buffer and assign the new color to it
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LEQUAL);
        // 
        //glFrontFace(GL_CCW);
        //glFrontFace(GL_CW);

        //glBindVertexArray(vao[0]);
            // --- need to keep track of which texture unit that is being used??????

                // WHY DON"T WE NEED THIS BECAUSE ONLY ONE TEXTURE????? MAYBE?????
                // if 2+ textures then need to activate and bind each before drawing
        //rTex->Bind();
        //tex.Bind();
                // WHY DON"T WE NEED THIS BECAUSE ONLY ONE TEXTURE????? MAYBE?????
        //glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "recvTexture"), 0);
       // tex.Bind();
       //glBindVertexArray(vao[0]);

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
            glDrawArrays(GL_TRIANGLES, 0, numIndices);
            break;

        default:
            break;
        }
        // Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

    glDeleteVertexArrays(1, vao);
    glDeleteBuffers(1, vbo);
    glDeleteBuffers(1, ebo);
    tex.~Texture();

	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

    return 0;
}
//-----------------------------------------
int DoCube(GLuint vbo, GLuint ebo)
{
        // 2 X 2 X 2 cube, each line a triangle, position data only
    //float vertsCube[108] = 
    //{
    //    -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
    //    1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
    //    1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
    //    1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
    //    1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    //    -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    //    -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
    //    -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
    //    -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
    //    1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
    //    -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    //    1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
    //};

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

        // return #vertices
    return 36;
}

//------------------------------------------------
int DoRectTexture(GLuint vbo, GLuint ebo)
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
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // single color #2 layout var
    glVertexAttrib4f(2, 0.86f, 0.9f, 0.32f, 1.0f);

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
int DoRect(GLuint vbo, GLuint ebo)
{
    float vertices[] = {
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
    glVertexAttrib4f(1, 0.86f, 0.9f, 0.32f, 1.0f);

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
            if (cmd == "1")
            {
                std::cout << "Lines->1 Cmd " << '\n';
                ArgvNum = std::stoi(cmd);
            }
            else if (cmd == "2")
            {
                std::cout << "Lines->2 Cmd " << '\n';
                ArgvNum = std::stoi(cmd);
            }
            else if (cmd == "3")
            {
                std::cout << "Lines->3 Cmd " << '\n';
                ArgvNum = std::stoi(cmd);
            }
            else if (cmd == "4")
            {
                std::cout << "Lines->4 Cmd " << '\n';
                ArgvNum = std::stoi(cmd);
            }

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
