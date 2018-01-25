#ifndef RENDERER_MANAGER_H_
#define RENDERER_MANAGER_H_

#include "quad_model.h"
#include "renderer_constants.h"
#include "renderer.h"
#include "scene_renderer.h"
#include "global.h"



// draw transparent entities
// we could sort these if it ever becomes a problem...


class RendererManager
{
	const int SHADOW_MAP_RATIO = 2;

    public:
        RendererManager();
        ~RendererManager();

        Renderer		r_fullVertexColor;
		Renderer		r_fullColor;
		Renderer		r_fullTexture;
		Renderer		r_playerTarget;

		Renderer		r_healthBar;

		Renderer		r_multiTexturedTerrain;

		Renderer	r_sceneColor;
		Renderer	r_sceneTexture;


		int shadowMapWidth;
		int shadowMapHeight;
		glm::mat4 m_lightViewProjMat;
		FrameBufferObject m_shadowMapFBO;

		FrameBufferObject m_backGroundLayerFBO;
		FrameBufferObject m_particleLayerFBO;

		void init(int width, int height);

	private:
		void initShadowMapFBO(int w, int h);
};

#endif



/*

#ifndef RENDERER_MANAGER_H_
#define RENDERER_MANAGER_H_

#include "quad_model.h"
#include "renderer.h"
#include "scene_renderer.h"
#include "light/light_manager.h"
class RendererManager
{
public:
RendererManager();
~RendererManager();


static Renderer		r_fullVertexColor;
static Renderer		r_fullColor;
static Renderer		r_fullTexture;
static Renderer		r_playerTarget;



Renderer r_healthBar;


static Renderer r_multiTexturedTerrain;
static SceneRenderer r_multiTexTerrain;
static SceneRenderer r_texturedObject;
static SceneRenderer r_billboardOneQuad;
static SceneRenderer r_billboardTwoQuad;


static void init();
static void initSceneRendererStaticLightsData(LightManager lightManager);
};

#endif



*/