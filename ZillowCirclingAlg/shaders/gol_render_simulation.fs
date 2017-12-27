#version 330
uniform sampler2D u_simulationTexture;

in vec2 vf_UV;
out vec4 FragColor;

void main()
{
	vec2 tc = vf_UV;	 tc.y = 1.0 - tc.y;

	vec3 color = vec3(1.0, 1.0, 1.0) - texture(u_simulationTexture, tc).xyz;
	FragColor = vec4(color, 1.0);
}


