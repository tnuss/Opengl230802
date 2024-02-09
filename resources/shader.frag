#version 450

in vec4 fs_color;
//in vec2 texCoords;

out vec4 color;
//uniform mat4 mv_matrix;
//uniform mat4 proj_matrix;

void main(void)
{
    //color = vec4(0.9, 0.4, 0.0, 1.0);
    color = fs_color;
}