
#include "renderer_manager.h"

RendererManager::RendererManager()
{

}

RendererManager::~RendererManager()
{

}


void RendererManager::init()
{
    Shader* s;

    s = new Shader("gol_user_input_with_pattern.vs", "gol_user_input_with_pattern.fs");
    r_GOLUserInputWithPattern.addShader(s);
    r_GOLUserInputWithPattern.addDataPair(RENDER_PASS1, "u_boardTexture", DP_INT);
    r_GOLUserInputWithPattern.addDataPair(RENDER_PASS1, "u_patternTexture",       DP_INT);
    r_GOLUserInputWithPattern.addDataPair(RENDER_PASS1, "u_patternBottomRightX",  DP_INT);
    r_GOLUserInputWithPattern.addDataPair(RENDER_PASS1, "u_patternBottomRightY",  DP_INT);
    r_GOLUserInputWithPattern.addDataPair(RENDER_PASS1, "u_patternWidth",         DP_INT);
    r_GOLUserInputWithPattern.addDataPair(RENDER_PASS1, "u_patternHeight",        DP_INT);
    r_GOLUserInputWithPattern.addDataPair(RENDER_PASS1, "u_mouseLeftBtnDown",   DP_BOOL);


    s = new Shader("gol_update.vs", "gol_update.fs");
    r_GOLUpdate.addShader(s);
    r_GOLUpdate.addDataPair(RENDER_PASS1, "u_sourceTexture", DP_INT);


    s = new Shader("gol_render_input_with_pattern.vs", "gol_render_input_with_pattern.fs");
    r_GOLRenderInputWithPattern.addShader(s);
    r_GOLRenderInputWithPattern.addDataPair(RENDER_PASS1, "u_inputTexture",         DP_INT);
    r_GOLRenderInputWithPattern.addDataPair(RENDER_PASS1, "u_patternTexture",       DP_INT);
    r_GOLRenderInputWithPattern.addDataPair(RENDER_PASS1, "u_patternBottomRightX",  DP_INT);
    r_GOLRenderInputWithPattern.addDataPair(RENDER_PASS1, "u_patternBottomRightY",  DP_INT);
    r_GOLRenderInputWithPattern.addDataPair(RENDER_PASS1, "u_patternWidth",         DP_INT);
    r_GOLRenderInputWithPattern.addDataPair(RENDER_PASS1, "u_patternHeight",        DP_INT);


    s = new Shader("gol_render_simulation.vs", "gol_render_simulation.fs");
    r_GOLRenderSimluation.addShader(s);
    r_GOLRenderSimluation.addDataPair(RENDER_PASS1, "u_simulationTexture",    DP_INT);


    s = new Shader("gol_render_intermediate.vs", "gol_render_intermediate.fs");
    r_GOLRenderIntermediate.addShader(s);
    r_GOLRenderIntermediate.addDataPair(RENDER_PASS1, "u_sourceTexture",    DP_INT);
    r_GOLRenderIntermediate.addDataPair(RENDER_PASS1, "u_srcPresentValue",  DP_INT);
    r_GOLRenderIntermediate.addDataPair(RENDER_PASS1, "u_srcEmptyValue",    DP_INT);
    r_GOLRenderIntermediate.addDataPair(RENDER_PASS1, "u_dstPresentValue",  DP_INT);
    r_GOLRenderIntermediate.addDataPair(RENDER_PASS1, "u_dstEmptyValue",    DP_INT);

}

