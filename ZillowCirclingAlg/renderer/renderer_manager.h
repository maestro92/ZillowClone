#ifndef RENDERER_MANAGER_H_
#define RENDERER_MANAGER_H_

#include "quad_model.h"
#include "renderer.h"
#include "Rect.h"

class RendererManager
{
    public:
        RendererManager();
        ~RendererManager();

        Renderer  r_GOLUpdate;
        Renderer  r_GOLUserInputWithPattern;
        Renderer  r_GOLRenderInputWithPattern;

        Renderer  r_GOLRenderIntermediate;
        Renderer  r_GOLRenderSimluation;

        void init();
};

#endif

