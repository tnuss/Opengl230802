#version 450

layout (location=0) in vec3 position;
//layout (location=1) in vec3 aColor;
layout (location=1) in vec4 aColor;
layout (location = 2) in vec2 aOffset;

//uniform mat4 mv_matrix;
//uniform mat4 proj_matrix;

out vec4 fs_color;

void main(void)
{
    gl_Position = vec4(position.x + aOffset.x, position.y + aOffset.y, position.z,1.0);
    fs_color = aColor;
}
