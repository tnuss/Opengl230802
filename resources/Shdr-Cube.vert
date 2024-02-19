#version 450 core
  // works for shapes 3

layout (location=0) in vec3 aPos;
layout (location=1) in vec4 aColor;

//uniform mat4 mv_matrix;
//uniform mat4 proj_matrix;
uniform mat4 scale;

out vec4 fs_color;

void main(void)
{
    //gl_Position = vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z,1.0);
    //gl_Position = vec4(aPos.x, aPos.y, aPos.z,1.0);
    gl_Position = scale * vec4(aPos,1.0);

    //fs_texCoords = aTexture;
    fs_color = aColor;
}
