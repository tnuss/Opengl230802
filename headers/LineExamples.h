#pragma once

#include <iostream>
#include <vector>
#include<glad/glad.h>
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp> 
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

//extern void Lines0(GLuint);
class LineExamples {

private:
    static const int numHexVertices = 6;

    int numVertices = 0;
    bool doLineStrip = false;
    bool doLineLoop = false;
    int numInsts = 1;
    float leHalfpi = 0.0174532925;
    bool bIsFlatTop = true;

public:

    glm::vec3 modelHexVerts[6];

    LineExamples() {};
    inline int GetNumVertices() { return numVertices; };
    inline bool DoLineStrip() { return doLineStrip; };
    inline bool DoLineLoop() { return doLineLoop; };
    inline void SetNumInstances(int insts) { numInsts = insts; };
    inline int GetNumInstances() { return numInsts; };
    inline bool IsFlatTop() { return bIsFlatTop; };

    //-------------------------------------
    void linesXYZ(GLuint vbo)
    {
        GLfloat vertices[] =
        { //    COORDINATES                  /     COLORS           //
            -1.0f,  0.0f, 0.0f,   0.8f, 0.0f, 0.00f, 1.0f, // Lower left
             1.0f,  0.0f, 0.0f,   0.8f, 0.0f, 0.00f, 1.0f, // Lower r
             0.0f,  1.0f, 0.0f,   0.8f, 0.0f, 0.00f, 1.0f,  // up r
             0.0f, -1.0f, 0.0f,   0.8f, 0.0f, 0.00f, 1.0f  // up r
        };

        numVertices = 4;
        doLineStrip = true;
        doLineLoop = false;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
        glEnableVertexAttribArray(0);

        //// if the vertex data changes this would go in display loop YES/NO??
        //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(3 * sizeof(float)));
        //glEnableVertexAttribArray(1);
        // single color #1 layout var
        glVertexAttrib4f(1, 0.86f, 0.9f, 0.32f, 1.0f);

    }
    //----------------------------------------------------
    void CreateHexModel()
    {
        // hex line model, 1.0 length to vertex from 0,0
        //   2.0 length from vert.to opposite vertex
        // flat top

        float size = 1.0f;
        // flat top vs point top (30 degrees)
        float deg = 0;
        //float deg = 30;
        float b = 0;

        for (int i = 0; i < numHexVertices; i++)
        {
            // 1 degree = pi/180 radians
            b = leHalfpi * ((60 * i) - deg);
            // center is 0,0,0 in this case
            modelHexVerts[i].x = size * glm::cos(b);
            modelHexVerts[i].y = size * glm::sin(b);
            modelHexVerts[i].z = 0.0f;
        }
    }

    //----------------------------------------------------
    void InstHexLine(GLuint vboVertices, GLuint vboInstance)
    {
        CreateHexModel();

        // !!!!!!!!  scaled size of hex should be dependent on window(viewport?) resolution 
           // 1) SCALE Hex model 
            // model has: point vertex to origin is 1 , point vertex to point vertex is 2
            // this will be the set of points that will worked on by eash instance
        float hexScaleSize = 0.14f;

        glm::vec3 hexVerts[numHexVertices];
        for (int i = 0; i < numHexVertices; i++)
        {
            hexVerts[i] = glm::vec3(modelHexVerts[i]);
            hexVerts[i] *= hexScaleSize;
        }

            // translations will be the offsets that will applied to the scaled model hex 
            // for each instance
        std::vector<glm::vec3> translations;
        translations.reserve(numInsts);
        float offset = 0.0f;
        glm::vec3 translation = { 0.0f,0.0f,0.0f};

        //std::cout << "test: " << sizeof(test) << '\n';
        //std::cout << "translation: " << sizeof(translation) << '\n';
        
        // calc ORIGIN/CENTER of hex offset from upper left of NDC model
        // it should be first entry in translations

        bool doUp = IsFlatTop();
        float startX = -1.0f;
        float startY = 1.0f;
        glm::vec2 transIt = { 0.0f,0.0f };
        transIt.x = startX + hexScaleSize;
        transIt.y = startY - ((glm::sqrt(3.0f) / 2.0f) * hexScaleSize);

        translation.x = transIt.x;
        translation.y = transIt.y;
        translations.push_back(translation);
            
        // next hex will be down half a hex
        doUp = false;

            // will use previous transIt TO CALC CENTERS OF REST OF HEXES TO BE DRAWN
            //  THIS IS WHAT SHOUOLD GO IN SHADER

        for (int i = 1; i < numInsts; i++)
        {
           /* offset = >> 1)calculate where from 0, 0 to current hex placement is or
            2) move to starting positionand then do offsets from there
            */
            if (doUp)
            {
                // move last set of vertices up and over
                transIt.x = transIt.x + hexScaleSize * 3.0f / 2.0f;
                transIt.y = transIt.y + ((glm::sqrt(3.0f) / 2.0f) * hexScaleSize);
                doUp = false;
            }
            else
            {       // move last set of vertices down and over
                transIt.x = transIt.x + hexScaleSize * 3.0f / 2.0f;
                transIt.y = transIt.y - ((glm::sqrt(3.0f) / 2.0f) * hexScaleSize);
                doUp = true;
            }

            translation.x = transIt.x;
            translation.y = transIt.y;
            translations.push_back(translation);

        }

        //for (int i = 0; i < numInsts; i++)
        //{
        //    std::cout << "Translationsx: " << translations[i].x << '\n';
        //    std::cout << "Translationsy: " << translations[i].y << '\n';
        //    std::cout << "Translationsz: " << translations[i].z << '\n';

        //}

        //float test[3] = { 0.0,0.0,0.0 };
        ////glm::vec3 test1[9];
        //std::vector <glm::vec3> test1[9] ;
        //std::cout << "test: " << sizeof(test) << '\n';
        //std::cout << "test1: " << sizeof(test1) << '\n';
        //std::cout << "translation: " << sizeof(translation) << '\n';
        //std::cout << "translations: " << sizeof(translations.data()) << '\n';
        //std::cout << "translations9: " << (sizeof(translations) * 9) << '\n';

        //glm::vec3* a ;
        //a = translations.data();

        //std::cout << "translations.data: " << a[0].x << '\n';
        //std::cout << "translations.data: " << a[0].y << '\n';
        //std::cout << "translations.data: " << a[0].z << '\n';
        ////a++;
        //std::cout << "translations.data: " << a[1].x << '\n';
        //std::cout << "translations.data: " << a[1].y << '\n';
        //std::cout << "translations.data: " << a[1].z << '\n';

        //for (int i = 0; i < 6; i++)
        //{
        //    std::cout << "hexVerts: " << hexVerts[i].x << '\n';
        //    std::cout << "hexVerts: " << hexVerts[i].y << '\n';
        //}

        numVertices = numHexVertices;
        doLineStrip = false;
        doLineLoop = true;

        glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(hexVerts), hexVerts, GL_STATIC_DRAW);
        //// if the vertex data changes this would go in display loop YES/NO??
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
        glEnableVertexAttribArray(0);

        // single color #1 layout var
        glVertexAttrib4f(1, 0.86f, 0.9f, 0.32f, 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vboInstance);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numInsts, &translations[0], GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(translations), &translations[0], GL_STATIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        //(GL_ARRAY_BUFFER, ++vbo); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
        glEnableVertexAttribArray(2);

    }
    //----------------------------------------------------
    void HexLine(GLuint vbo)
    {
        CreateHexModel();

        // !!!!!!!!  size of hex is dependent on window(viewport?) resolution 
           // 1) SCALE Hex model 
            // model: point vertex to origin is 1 , point to point is 2
        float hexScaleSize = 0.1f; 

        glm::vec3 hexVerts[numHexVertices + 12];
        for (int i=0; i < numHexVertices; i++)
        {
            hexVerts[i] = glm::vec3(modelHexVerts[i]);
            hexVerts[i] *= hexScaleSize;
        }

            // 2) TRANSLATE the scaled Hex model
        //struct trans {
        //    float tX;
        //    float tY;
        //    float tZ = 1;
        //} transIt;


        //transM4 = glm::translate(transM4, glm::vec3( 1,1,1 ));

         // calc origin/center of hex offset from upper left of NDC model
        // set beginning offset for screen
        // set up for vec3 version of translation
        float startX = -1.0f;
        float startY = 1.0f;
        glm::vec3 transIt = { 0.0f,0.0f,1.0f };
        transIt.x= startX + hexScaleSize;
        transIt.y = startY - ((glm::sqrt(3.0f) / 2.0f) * hexScaleSize);

                // VEC3 and LOOP translation -- 2nd version of translation 
                // for 2D seems more straight forward than using glm::translate/mat4
                // ...maybe could have precision trouble vs using straight glm constructs
        for (int i = 0; i < numHexVertices; i++)
        {
            hexVerts[i].x += transIt.x;
            hexVerts[i].y += transIt.y;
            hexVerts[i].z += transIt.z;
        }

            // second hex move first set of vertices down and over
        transIt.x = hexScaleSize * 3.0f / 2.0f;
        transIt.y = ((glm::sqrt(3.0f) / 2.0f) * hexScaleSize);

        for (int i = numHexVertices; i < numHexVertices + 6; i++)
        {
            hexVerts[i].x = hexVerts[i - 6].x + transIt.x;
            hexVerts[i].y = hexVerts[i - 6].y - transIt.y;
            hexVerts[i].z = transIt.z;
        }

        // move hex move 2nd set of vertices up and over
        transIt.x = hexScaleSize * 3.0f / 2.0f;
        transIt.y = ((glm::sqrt(3.0f) / 2.0f) * hexScaleSize);

        for (int i = numHexVertices + 6; i < numHexVertices + 12; i++)
        {
            hexVerts[i].x = hexVerts[i - 6].x + transIt.x;
            hexVerts[i].y = hexVerts[i - 6].y + transIt.y;
            hexVerts[i].z = transIt.z;
        }

        numVertices = numHexVertices;
        doLineStrip = false;
        doLineLoop = true;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(hexVerts), hexVerts, GL_STATIC_DRAW);

        //// if the vertex data changes this would go in display loop YES/NO??
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(0);

                        // single color #1 layout var
        glVertexAttrib4f(1, 0.86f, 0.9f, 0.32f, 1.0f);

    }
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

