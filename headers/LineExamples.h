#pragma once

#include <iostream>
#include <vector>
#include<glad/glad.h>
#include <glm/glm.hpp> 
#include <glm/gtc/random.hpp>

//extern void Lines0(GLuint);
class LineExamples {

private:
    int numVertices = 0;
    bool doLineStrip = false;
    bool doLineLoop = false;
    int numInsts = 1;

public:
    LineExamples() {};
    inline int GetNumVertices() { return numVertices; };
    inline bool DoLineStrip() { return doLineStrip; };
    inline bool DoLineLoop() { return doLineLoop; };
    inline void SetNumInstances(int insts) { numInsts = insts; };
    inline int GetNumInstances() { return numInsts; };

    //-----------------------------------------
        // line with color defined by single glVertexAttrib4f(layout loc, vec4 color)
    void lines0(GLuint vbo)
    {

        GLfloat vertices[] =
        { //               COORDINATES                  /     COLORS           //
            -0.25f, -0.25f, 0.0f,    // 0.8f, 0.3f,  0.02f, 1.0// Lower left
             0.30f,  -0.25f, 0.0f,    // 0.8f, 0.3f,  0.02f, 1.0// Lower r
             0.25f,  0.25f, 0.0f,    // 0.8f, 0.3f,  0.02f, 1.0// up r
        };

        numVertices = 3;
        doLineStrip = true;
        doLineLoop = false;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        //// if the vertex data changes this would go in display loop YES/NO??
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(0);

          // location 1 color
        //glVertexAttrib4f(1, 0.5f, 1.0f, 0.2f, 1.0f);		// A greenish color (R, G, B, alpha values).
        glVertexAttrib4f(1, 0.86f, 0.9f, 0.32f, 1.0f);

    }

    //-----------------------------------------
    void lines1(GLuint vbo)
    {
        GLfloat vertices[] =
        { //               COORDINATES                  /     COLORS           //
            -0.25f, -0.25f, 0.0f,    0.8f, 0.3f,  0.02f, 1.0f, // Lower left
             0.5f,  -0.25f, 0.0f,    0.4f, 0.9f,  0.72f, 1.0f, // Lower r
             0.25f,  0.25f, 0.0f,    0.6f, 0.6f,  0.32f, 1.0f  // up r
        };

        numVertices = 3;
        doLineStrip = true;
        doLineLoop = false;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
        glEnableVertexAttribArray(0);

        //// if the vertex data changes this would go in display loop YES/NO??
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

    }

    //-----------------------------------------
    void lines2(GLuint vbo)
    {
        GLfloat vertices[] =
        { //               COORDINATES                  /     COLORS           //
            -0.90f, -0.90f, 0.0f,    0.8f, 0.3f,  0.02f, 1.0f, // Lower left
             0.90f, -0.90f, 0.0f,    0.4f, 0.9f,  0.72f, 1.0f, // Lower r
             0.90f,  0.90f, 0.0f,    0.6f, 0.6f,  0.32f, 1.0f,  // up r
            -0.90f,  0.90f, 0.0f,    0.6f, 0.75f,  0.45f, 1.0f  // up l
        };

        numVertices = 4;
        doLineStrip = false;
        doLineLoop = true;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
        glEnableVertexAttribArray(0);

        //// if the vertex data changes this would go in display loop YES/NO??
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

    }

    //-----------------------------------------
    void lines3(GLuint vbo) {
        
        // random lines instanced
        // instancing vars
        std::vector<glm::vec2> translations;
        translations.reserve(numInsts);
        int index = 0;
        float offset = 0.0f;
        glm::vec2 translation = { 0,0 };
        // glm uses c random generator...to change seed use srand
        std::srand(time(0));

        for (index = 0; index < numInsts; index++)
        {
            offset = glm::linearRand(-0.5f, 0.5f);
            //offset = glm::gaussRand(0.5f, 0.05f);
            translation.x = offset;
            offset = glm::linearRand(-0.5f, 0.5f);
            //offset = glm::gaussRand(0.5f, 0.05f);
            translation.y = offset;

            //translation = glm::circularRand(0.5);
            translations.push_back(translation);
        }
                // set line to be instanced
        lines0(vbo);

        // also set instance data
        //glGenBuffers(1, &vbo[2]);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numInsts, &translations[0], GL_STATIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        //glBindBuffer(GL_ARRAY_BUFFER, vbo[2]); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
        glEnableVertexAttribArray(2);

    }

};

