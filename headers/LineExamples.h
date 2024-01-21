#pragma once

#include <iostream>
#include<glad/glad.h>

//extern void Lines0(GLuint);
class LineExamples {

private:
    int numVertices = 0;
    bool doLineStrip = false;
    bool doLineLoop = false;

public:
    LineExamples() {};
    inline int GetNumVertices() { return numVertices; };
    inline bool DoLineStrip() { return doLineStrip; };
    inline bool DoLineLoop() { return doLineLoop; };

        // line with color defined by single glVertexAttrib4f(layout loc, vec4 color)
    void lines0(GLuint vbo)
    {

        GLfloat vertices[] =
        { //               COORDINATES                  /     COLORS           //
            -0.25f, -0.25f, 0.0f,    // 0.8f, 0.3f,  0.02f, 1.0// Lower left
             0.50f,  -0.25f, 0.0f,    // 0.8f, 0.3f,  0.02f, 1.0// Lower r
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

};

