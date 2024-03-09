#version 450 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec4 aColor;

//uniform mat4 scMatrix;;
uniform mat4 vs_mvmatrix;
uniform mat4 vs_projection;

//uniform mat4 proj_matrix;
//uniform mat4 scale_matrix;

out vec4 fs_color;

void main(void)
{
    //gl_Position = vs_projection * vs_mvmatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0) ;
    gl_Position = vs_mvmatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0) ;

    fs_color = aColor;
}
