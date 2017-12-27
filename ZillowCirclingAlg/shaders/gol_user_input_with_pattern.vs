#version 330

layout (location = 0) in vec3 position;
layout (location = 3) in vec2 UV;

out vec2 vf_UV;

void main()
{
    gl_Position = vec4(position, 1.0);
    vf_UV = UV;
}


