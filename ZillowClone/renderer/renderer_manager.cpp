
#include "renderer_manager.h"


RendererManager::RendererManager()
{

}

RendererManager::~RendererManager()
{

}


void RendererManager::init(int width, int height)
{
	char* filename = "renderer/renderer_data.json";

	Value vContent = utl::readJsonFileToVector(filename);
	const Array& vArray = vContent.get_array();

	Renderer::initRendererWrapper(vArray, &r_fullVertexColor, "r_fullVertexColor");
	Renderer::initRendererWrapper(vArray, &r_fullColor, "r_fullColor");
	Renderer::initRendererWrapper(vArray, &r_fullTexture, "r_fullTexture");
	Renderer::initRendererWrapper(vArray, &r_playerTarget, "r_playerTarget");

	m_backGroundLayerFBO = utl::createFrameBufferObject(width, height);
	m_particleLayerFBO = utl::createFrameBufferObject(width, height);
}


/*



void RendererManager::initShadowMapFBO(int w, int h)
{
	glGenTextures(1, &m_shadowMapFBO.depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapFBO.depthTexture);

	// No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// Remove artifact on the edges of the shadowmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Create a FBO and attach the depth texture:
	glGenFramebuffers(1, &m_shadowMapFBO.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO.FBO);

	// Instruct openGL that we won't bind a color texture with the currently bound FBO
	// Disable writes to the color buffer
	glDrawBuffer(GL_NONE);

	// Disable reads from the color buffer
	glReadBuffer(GL_NONE);

	// attach the texture to FBO depth attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapFBO.depthTexture, 0);

	// check FBO status
	utl::errorCheck();

	GLenum FBOstatus;
	FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (FBOstatus != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "GL_FRAMEBUFFER_COMPLETE failed. CANNOT use FBO" << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// don't need the color channel, just the dpeth

}

*/


void RendererManager::initShadowMapFBO(int w, int h)
{
	glGenTextures(1, &m_shadowMapFBO.depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_shadowMapFBO.depthTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Remove artifact on the edges of the shadowmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Create a FBO and attach the depth texture:
	glGenFramebuffers(1, &m_shadowMapFBO.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO.FBO);

	// Instruct openGL that we won't bind a color texture with the currently bound FBO
	// Disable writes to the color buffer
	glDrawBuffer(GL_NONE);

	// Disable reads from the color buffer
	glReadBuffer(GL_NONE);

	// attach the texture to FBO depth attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMapFBO.depthTexture, 0);

	// check FBO status
	utl::errorCheck();

	GLenum FBOstatus;
	FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (FBOstatus != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "GL_FRAMEBUFFER_COMPLETE failed. CANNOT use FBO" << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// don't need the color channel, just the dpeth

}

/*
void RendererManager::initSceneRendererStaticLightsData(LightManager lightManager)
{

	r_multiTexTerrain.enableShader();
		r_multiTexTerrain.setDirLightData(lightManager.getDirLight(0));
	r_multiTexTerrain.disableShader();

	r_texturedObject.enableShader();
		r_texturedObject.setDirLightData(lightManager.getDirLight(0));
	r_texturedObject.disableShader();

	r_texturedObject.enableShader();
		r_billboardOneQuad.setDirLightData(lightManager.getDirLight(0));
	r_texturedObject.disableShader();

	r_texturedObject.enableShader();
		r_billboardTwoQuad.setDirLightData(lightManager.getDirLight(0));
	r_texturedObject.disableShader();

}
*/










