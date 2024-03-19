#include "mesh.h"
//#include <iostream>
//#include <vector>

mesh::mesh(Vertex* vertices, unsigned int numVertices,
    unsigned int* indices, unsigned int numIndices)
    //mesh::mesh(Vertex* vertices, unsigned int numVertices)
{
    //unsigned int numInd = sizeof(*vertices) / sizeof(Vertex);
    //std::cout << "numInd: " << numInd << '\n';

    m_drawCount = numVertices;
    m_numIndices = numIndices;

    //vao
    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    if (m_numIndices > 0)
    {           // ********* TEMP 
       // glGenBuffers(1, m_VertexArrayBuffers);
       // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VertexArrayBuffers[0]);
              // ********* TEMP 
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices * sizeof(unsigned int), indices,
            GL_STATIC_DRAW);
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;

    positions.reserve(numVertices);
    texCoords.reserve(numVertices);

    for (int i = 0; i < numVertices; i++)
    {
        positions.push_back(*vertices[i].GetPos());
        texCoords.push_back(*vertices[i].GetTexCoord());
    }

    glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[0]);
    //glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(positions[0]),
    //    &vertices[0], GL_STATIC_DRAW);
          // notice using vertices ... positions and textures
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertices[0]), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(positions[0]), &positions[0]);
    glBufferSubData(GL_ARRAY_BUFFER, numVertices * sizeof(positions[0]), numVertices * sizeof(texCoords[0]), &texCoords[0]);
    //    // set data sequence attributes in vertices... pos, color, texCoord, etc..
    ////glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * sizeof(positions[0])));
    //glEnableVertexAttribArray(POSITION_VB);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);


    //glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
    //glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(texCoords[0]), 
    //             &texCoords[0], GL_STATIC_DRAW);
    //       // set data sequence attributes in vertices... pos, color, texCoord, etc..
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(TEXCOORD_VB);

    //glBindVertexArray(0);

}

mesh::~mesh()
{
    //glad_glDeleteVertexArrays(1, &m_vertexArrayObject);

    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void mesh::Draw()
{
    glBindVertexArray(m_vertexArrayObject);
    //glBindVertexArray(m_elementArrayBuffers[0]);

    if (m_meshdrawtype == USE_ARRAY)
        glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
    else
        if (m_meshdrawtype == USE_INDICES)
            glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);

    //glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
    //glDrawArrays(GL_LINE_LOOP, 0, m_drawCount);

    //glBindVertexArray(0);
}
