#version 330

uniform sampler2D u_inputTexture;
uniform sampler2D u_patternTexture;

uniform int u_patternBottomRightX;
uniform int u_patternBottomRightY;

uniform int u_patternWidth;
uniform int u_patternHeight;

in vec2 vf_UV;
out vec4 FragColor;

void main()
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy);

	int xdiff = u_patternBottomRightX - fragCoord.x;
	int ydiff = fragCoord.y - u_patternBottomRightY;
	
 	bool bx = (0 < xdiff) && (xdiff <= u_patternWidth);
 	bool by = (0 <= ydiff) && (ydiff < u_patternHeight);
 	
 	vec2 tc = vf_UV;	tc.y = 1.0 - tc.y;
 	vec4 bg = texture(u_inputTexture, tc);

 	if(bx && by)
 	{
 		if(bg.r == 0)
 			FragColor = bg;
 		else
 			FragColor = texelFetch(u_patternTexture, ivec2(u_patternWidth - xdiff, u_patternHeight - ydiff - 1), 0);		
 	}
 	else
 	{
 		 FragColor = bg;
 	}
}


