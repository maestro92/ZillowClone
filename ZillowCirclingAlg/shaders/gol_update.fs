#version 330

out vec4 FragColor;
in vec2 vf_UV;

uniform sampler2D u_sourceTexture;

void main()
{
    ivec2 T = ivec2(gl_FragCoord.xy);

    ivec2 cNW = T + ivec2(-1, 1),	cN = T + ivec2(0, 1),	cNE = T + ivec2(1, 1);
    ivec2 cW  = T + ivec2(-1, 0),	c  = T + ivec2(0, 0),	cE  = T + ivec2(1, 0);
    ivec2 cSW = T + ivec2(-1, -1),	cS = T + ivec2(0, -1),	cSE = T + ivec2(1, -1);

/*
  	T = T + oW;

    int pNW = int(texelFetchOffset(u_sourceTexture, T, 0, oNW).r);		
    int pN  = int(texelFetchOffset(u_sourceTexture, T, 0, oN).r);				
    int pNE = int(texelFetchOffset(u_sourceTexture, T, 0, oNE).r);		
    
    int pW  = int(texelFetchOffset(u_sourceTexture, T, 0, oW).r);		
    int p   = int(texelFetchOffset(u_sourceTexture, T, 0, o).r);		
    int pE  = int(texelFetchOffset(u_sourceTexture, T, 0, oE).r);		
    
    int pSW = int(texelFetchOffset(u_sourceTexture, T, 0, oSW).r);		
    int pS  = int(texelFetchOffset(u_sourceTexture, T, 0, oS).r);				
    int pSE = int(texelFetchOffset(u_sourceTexture, T, 0, oSE).r);		
  */
    
    int pNW = int(texelFetch(u_sourceTexture, cNW, 0).r);		
    int pN  = int(texelFetch(u_sourceTexture, cN, 0).r);				
    int pNE = int(texelFetch(u_sourceTexture, cNE, 0).r);		
    
    int pW  = int(texelFetch(u_sourceTexture, cW, 0).r);		
    int p   = int(texelFetch(u_sourceTexture, c, 0).r);		
    int pE  = int(texelFetch(u_sourceTexture, cE, 0).r);		
    
    int pSW = int(texelFetch(u_sourceTexture, cSW, 0).r);		
    int pS  = int(texelFetch(u_sourceTexture, cS, 0).r);				
    int pSE = int(texelFetch(u_sourceTexture, cSE, 0).r);	
    int sum = pNW + pN + pNE + pW + pE + pSW + pS + pSE;

    if(p == 1)
    {
        if(sum == 3 || sum == 2)
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);    	

        // under population and over population
        else
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        if(sum == 3)
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}