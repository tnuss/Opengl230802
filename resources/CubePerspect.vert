#version 450 core
  // works for shapes #5

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexture;
layout (location=2) in vec4 aColor;

//uniform mat4 mv_matrix;
//uniform mat4 proj_matrix;
uniform mat4 vsMVMatrix;
uniform mat4 vsProjMatrix;
//uniform mat4 vs_mvmatrix;

out vec4 fs_color;
out vec2 fs_texCoords;

void main(void)
{
    //gl_Position = vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z,1.0);
    //gl_Position = vec4(aPos.x, aPos.y, aPos.z,1.0);
           // offset it a little
    gl_Position = vsProjMatrix * vsMVMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    fs_texCoords = aTexture;
    fs_color = aColor;
}
