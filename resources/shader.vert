#version 450 core
  // works for shapes 1, 2

layout (location=0) in vec3 aPos;
       // shapes 1 and 2 (maybe others) are mismatching loc 1 and 2 for color and texture
layout (location=1) in vec4 aColor;
layout (location=2) in vec2 aTexture;
//layout (location=2) in vec4 aColor;
//layout (location = 2) in vec2 aOffset;

uniform uint vsShapeSwitch = 0;
//uniform mat4 proj_matrix;

out vec2 fs_texCoords;
out vec4 fs_color;

void main(void)
{
    //gl_Position = vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z,1.0);
    gl_Position = vec4(aPos.x, aPos.y, aPos.z,1.0);

    if (vsShapeSwitch == 1)
    {
        fs_texCoords = aTexture;
    }
    fs_texCoords = aTexture;
    fs_color = aColor;
}
