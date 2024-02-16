#version 450

in vec2 fs_texCoords;
in vec4 fs_color;

out vec4 color;

uniform sampler2D recvTexture;
//uniform mat4 mv_matrix;
//uniform mat4 proj_matrix;

void main(void)
{
    //color = vec4(0.9, 0.4, 0.0, 1.0);
   //gl_FragColor = texture(recvTexture, fs_texCoords) ;
   //color = texture(recvTexture, fs_texCoords) ;
   color = fs_color;
}