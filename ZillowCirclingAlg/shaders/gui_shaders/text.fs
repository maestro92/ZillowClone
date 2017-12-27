#version 330

uniform sampler2D u_texture;
uniform vec3 u_color;

in vec2 vf_UV;
out vec4 FragColor;

void main()
{
	vec2 tc = vf_UV;

	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_texture, tc).r);
	FragColor = vec4(u_color, 1.0) * sampled;
}
