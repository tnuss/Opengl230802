#version 450

layout (location=0) in vec3 position;
layout (location=1) in vec4 aColor;
//layout (location = 2) in mat4 transM;
layout (location = 2) in vec3 v3Offset;

//uniform mat4 mv_matrix;
//uniform mat4 proj_matrix;
//uniform mat3 id_matrix;
mat4 transMatrix;

out vec4 fs_color;

mat4 translate(vec3);

void main(void)
{
	transMatrix = translate(v3Offset);
	gl_Position = transMatrix * vec4(position,1.0);
	
    fs_color = aColor;
}

//--------------------------------------------------
mat4 translate(vec3 d)
{
	return mat4(1, 0, 0, 0,
	            0, 1, 0, 0,
	            0, 0, 1, 0,
	            d.x, d.y, d.z, 1);
	           // -0.86, 0.8787, 0, 1); these are from offset?? they make the lines go out of window
};
