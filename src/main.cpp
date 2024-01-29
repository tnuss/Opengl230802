#include<iostream>
#include<string>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

//#include<stb/stb_image.h>

#include "LineExamples.h"
#include "Shader.h"

const int numVAOs = 1;
const int numVBOs = 3;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

//--------- Funct Declarations

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

GLFWwindow* InitGladAndWindow();

//----------------------------------------------
       // default argv
int linesCMD = 4;
    // line examples 'class'
LineExamples linesInst;

int DoCmdLine(int argc, char* argv[])
{
    std::cout << "Opengl Lines" << '\n';
    std::cout << "CMD #Args: " << argc << '\n';

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
                linesCMD = std::stoi(cmd);
            }
            else if (cmd == "2")
            {
                std::cout << "Lines->2 Cmd " << '\n';
                linesCMD = std::stoi(cmd);
            }
            else if (cmd == "3")
            {
                std::cout << "Lines->3 Cmd " << '\n';
                linesCMD = std::stoi(cmd);
            }
            else if (cmd == "4")
            {
                std::cout << "Lines->4 Cmd " << '\n';
                linesCMD = std::stoi(cmd);
            }
        }
    }
    return 0;
}

int DoLineExamples()
{
  
    // line with color defined by single glVertexAttrib4f(layout loc, vec4 color)
    if (linesCMD == 0)
    {
        // set and activate(bind) the graphics buffers
        linesInst.lines0(vbo[0]);

    }

    // line with color defined by using color per vertex via vbo
// ----------- 1
//// if the vertex data changes this would go in display loop YES/NO??
    if (linesCMD == 1)
    {
        linesInst.lines1(vbo[2]);
        //glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
        //glEnableVertexAttribArray(0);

        ////// if the vertex data changes this would go in display loop YES/NO??
        //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(3 * sizeof(float)));
        //glEnableVertexAttribArray(1);

    }

    // line with color defined by using color per vertex via vbo...outlines window to a degree
    if (linesCMD == 2)
    {
        // set and activate(bind) the graphics buffers
        linesInst.lines2(vbo[0]);
    }

    if (linesCMD == 3)
    {
        // multiple lines via instancing
        //int numInsts = 9;
        // set and activate(bind) the graphics buffers, 
        linesInst.SetNumInstances(9);
        linesInst.lines3(vbo[0]);
    }

    if (linesCMD == 4)
    {
        //Draw Hex Line
        
        linesInst.HexLine(vbo[0]);

        //linesInst.SetNumInstances(9);
        //linesInst.lines3(vbo[0]);
    }

    return 0;
}

//----------------------------------------------
int main(int argc, char* argv[])
{
    DoCmdLine(argc, argv);
    
    glfwInit();

    GLFWwindow* window = InitGladAndWindow();
    if (window == NULL)
        return -1;

    Shader shader("resources/shader");
    shader.Bind();
 
    glGenVertexArrays(numVAOs, vao);
    glGenBuffers(numVBOs, vbo);
        
    // clear then activate vao
    glBindVertexArray(0);
    glBindVertexArray(vao[0]);

    DoLineExamples();

	    // Main while loop
	while (!glfwWindowShouldClose(window))
	{
        processInput(window);
		// Specify the color of the background
		//  glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // note: have to worry about GL_MAX_VERTEX_ATTRIBS !!!!
        
            // doesn't work with 2D Lines????
        //glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LEQUAL);

        // this seems to have a big effect
        //glFrontFace(GL_CW);
        //glFrontFace(GL_CCW);

        glLineWidth(2.0f);

        if (linesInst.DoLineStrip())
            //glDrawArrays(GL_LINE_STRIP, 0, linesInst.GetNumVertices());
            glDrawArraysInstanced(GL_LINE_STRIP, 0, linesInst.GetNumVertices(), 
                                   linesInst.GetNumInstances());
        else if (linesInst.DoLineLoop())
            glDrawArrays(GL_LINE_LOOP, 0, linesInst.GetNumVertices());

            // ******  if data changes uncomment this?????
        //glDisableVertexAttribArray(0);
        //glDisableVertexAttribArray(1);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

        // clean up
    glBindVertexArray(0);

	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

    return 0;

}

//----------------------------------------------

GLFWwindow* InitGladAndWindow()
{
    //
     //	// Tell GLFW what version of OpenGL we are using 
     //	// In this case we are using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Tell GLFW we are using the CORE profile
    // So that means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    //	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
    GLFWwindow* window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL);

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
    glViewport(0, 0, 800, 800);

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
