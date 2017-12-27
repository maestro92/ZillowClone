#version 330

uniform vec2 u_inputPosition;

uniform sampler2D u_boardTexture;
uniform float u_startGridX;
uniform float u_startGridY;
uniform float u_endGridX;
uniform float u_endGridY;
uniform float u_invWidth;
uniform float u_invHeight;

uniform bool u_mouseLeftBtnDown;

in vec2 vf_UV;
out vec4 FragColor;

void main()
{
	vec2 fragCoord = gl_FragCoord.xy;
// 	bool bx = (u_startGridX < fragCoord.x) && (fragCoord.x <= u_endGridX);
// 	bool by = (u_startGridY <= fragCoord.y) && (fragCoord.y < u_endGridY);

 	bool bx = (u_startGridX <= fragCoord.x) && (fragCoord.x < u_endGridX);
 	bool by = (u_startGridY <= fragCoord.y) && (fragCoord.y < u_endGridY);
 	if(bx && by && u_mouseLeftBtnDown)
 		FragColor = vec4(0.0,0.0,0.0,1.0);
	else
	{

		// vec2 tc = vec2(1.0, 1.0);
		// tc.x = fragCoord.x * u_invWidth;
		// tc.y = fragCoord.y * u_invHeight;
		// vec4 color = texture(u_boardTexture, tc);
		// FragColor = color;
	 	vec2 tc = vf_UV;	 tc.y = 1.0 - tc.y;
	 	FragColor = texture(u_boardTexture, tc);
	}
/*
 	if(fragCoord.x < u_inputPosition.x && fragCoord.y < u_inputPosition.y)
	// if(fragCoord.x < tempPos.x && fragCoord.y < tempPos.y)	
		FragColor = vec4(0.0,0.0,0.0,1.0);

	else
		FragColor = vec4(1.0,1.0,1.0,1.0);
*/
}



