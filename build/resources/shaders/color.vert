#version 330 core
//3,2
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aColor;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};
uniform mat4 model;

out vec4 gl_Position;
out vec3 color;

void main()
{
   gl_Position = proj * view * model * vec4(aPos, 1.0);
   color = vec3(aColor, 1.0f);
}