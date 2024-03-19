#version 450 core

  // works for shapes 1, 2
in vec2 fs_texCoords;
in vec4 fs_color;

uniform uint vsShapeSwitch = 0;

out vec4 color;

layout (binding=0) uniform sampler2D recvTexture;
//uniform sampler2D recvTexture;
//uniform mat4 mv_matrix;
//uniform mat4 proj_matrix;

void main(void)
{
    //color = vec4(0.9, 0.4, 0.0, 1.0);
   //gl_FragColor = texture(recvTexture, fs_texCoords) ;
         // shape 2 uses texture
   //color = texture(recvTexture, fs_texCoords) ;
         // shape 1 uses color
   if (vsShapeSwitch == 1)
      color = fs_color;
   else
      if (vsShapeSwitch == 2)
         color = texture(recvTexture, fs_texCoords) ;

}