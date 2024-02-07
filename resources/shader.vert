#version 450
//#version 430

layout (location=0) in vec3 position;
//layout (location=1) in vec3 aColor;
layout (location=1) in vec4 aColor;
//in vec3 position;
//layout (location=1) in vec2 texCoords;
layout (location = 2) in vec2 aOffset;

//uniform mat4 mv_matrix;
//uniform mat4 proj_matrix;

//out vec4 gl_Position;
out vec4 fs_color;
//out vec2 texCoords;

void main(void)
{ 
    gl_Position = vec4(position.x + aOffset.x, position.y + aOffset.y, position.z,1.0);
    //gl_Position = mv_matrix * vec4(position,1.0);
    //fs_color = vec4(position,1.0) * 0.5 + vec4(0.5,0.5,0.5,0.5);
    //fs_color = vec4(aColor, 1.0);
    fs_color = aColor;

}


