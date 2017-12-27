#include "game_board.h"


/*


*/
GameBoard::GameBoard(int w, int h, int s)
{
    if(w%s != 0 || h%s !=0)
    {
        cout << " GameBoard size is not a factor of width or height" << endl;
        exit(1);
    }

    setWidth(w);
    setHeight(h);
    setGridSize(s);

    m_numGridsX = w/s;
    m_numGridsY = h/s;

    m_inverseWidth = 1/(float)w;
    m_inverseHeight = 1/(float)h;

    /*

        pipeline temp_pipeline;
        temp_pipeline.loadIdentity();
        temp_pipeline.ortho(-1,1,-1,1,-1,1);

        the parameters of the ortho should match the dimensions of the quadModel
        if quadModel is init    left:0  right:1
                                bot:0   top:1

        then the ortho should also be
                                left:0  right:1
                                bot:0   top:1
    */
    m_boardQuadModel.init(-1, 1, -1, 1,
                          0.0, 0.0, 1.0);

    m_userInputBoardDoubleBuffer = Utility::createDoubleFrameBufferObject(m_width, m_height);
    m_userInputBoardDoubleBuffer.clear(glm::vec4(1.0,1.0,1.0,1.0));

    m_simulationDoubleBuffer = Utility::createDoubleFrameBufferObject(m_numGridsX, m_numGridsY);
    m_simulationDoubleBuffer.clear();



    m_inputToSimulationRenderInfo.set(m_numGridsX, m_numGridsY,
                                        m_userInputBoardDoubleBuffer.ping.colorTexture,
                                        m_simulationDoubleBuffer.ping.FBO, 0, 1);
 //   m_inputToScreenRenderInfo.set(m_width, m_height, RENDER_TO_SCREEN, 0, 0);

    m_simulationToInputRenderInfo.set(m_width, m_height,
                                        m_simulationDoubleBuffer.ping.colorTexture,
                                        m_userInputBoardDoubleBuffer.ping.FBO, 1, 0);
  //  m_simulationToScreenRenderInfo.set(m_width, m_height, RENDER_TO_SCREEN, 1, 0);
}

void GameBoard::setWidth(int w)
{
    m_width = w;
}

void GameBoard::setHeight(int h)
{
    m_height = h;
}

void GameBoard::setGridSize(int s)
{
    m_gridSize = s;
    m_invGridSize = 1/(float)m_gridSize;
}

int GameBoard::getWidth()
{
    return m_width;
}

int GameBoard::getHeight()
{
    return m_height;
}

int GameBoard::getGridSize()
{
    return m_gridSize;
}
/*
glm::vec2 GameBoard::screenCoordToBoardCoord(glm::vec2 pos)
{
    glm::vec2 bCoord;
    bCoord.x = floor(pos.x * m_invGridSize);
    bCoord.y = floor(pos.y * m_invGridSize);
    return bCoord;
}
*/
void GameBoard::initUserInput(Renderer* renderer, MouseState& mouseState, GOLModel* pattern)
{
    glViewport(0, 0, m_width, m_height);
    glBindFramebuffer(GL_FRAMEBUFFER, m_userInputBoardDoubleBuffer.pong.FBO);

    glm::vec2 spos = screenCoordToBoardCoord(mouseState.m_pos);
    int sx = (int)(spos.x * m_gridSize);
    int sy = (int)(spos.y * m_gridSize);

    int ex = (int)(sx + m_gridSize);
    int ey = (int)(sy + m_gridSize);
    pipeline temp_pipeline;
    temp_pipeline.loadIdentity();
    temp_pipeline.ortho(-1,1,-1,1,-1,1);

    bool mouseFlag = mouseState.m_leftButtonDown;
    if(mouseState.m_pos.x > m_width)
        mouseFlag = false;


    renderer->enableShader(RENDER_PASS1);
        renderer->setData(RENDER_PASS1, "u_boardTexture", 0,
                                        GL_TEXTURE0,
                                        m_userInputBoardDoubleBuffer.ping.colorTexture);

        renderer->setData(RENDER_PASS1, "u_patternTexture", 1,
                                        GL_TEXTURE1,
                                        pattern->m_patternTexture);

        renderer->setData(RENDER_PASS1, "u_patternBottomRightX", ex);
        renderer->setData(RENDER_PASS1, "u_patternBottomRightY", sy);
        renderer->setData(RENDER_PASS1, "u_patternWidth", pattern->m_width);
        renderer->setData(RENDER_PASS1, "u_patternHeight", pattern->m_height);
//        renderer->setData(RENDER_PASS1, "u_mouseLeftBtnDown", mouseState.m_leftButtonDown);
        renderer->setData(RENDER_PASS1, "u_mouseLeftBtnDown", mouseFlag);
//        renderer->setData(RENDER_PASS1, "u_mouseLeftBtnDown", true);
        renderer->loadUniformLocations(temp_pipeline, RENDER_PASS1);
        m_boardQuadModel.render();

    renderer->disableShader(RENDER_PASS1);
}


void GameBoard::update(Renderer* renderer)
{
    glViewport(0, 0, m_numGridsX, m_numGridsY);
    glBindFramebuffer(GL_FRAMEBUFFER, m_simulationDoubleBuffer.pong.FBO);

    pipeline temp_pipeline;
    temp_pipeline.loadIdentity();
    temp_pipeline.ortho(-1,1,-1,1,-1,1);

    renderer->enableShader(RENDER_PASS1);
        renderer->setData(RENDER_PASS1, "u_sourceTexture", 0, GL_TEXTURE0, m_simulationDoubleBuffer.ping.colorTexture);
        renderer->loadUniformLocations(temp_pipeline, RENDER_PASS1);
        m_boardQuadModel.render();
    renderer->disableShader(RENDER_PASS1);
}


void GameBoard::renderInput(Renderer* renderer, MouseState& mouseState, GOLModel* pattern)
{
    glViewport(0, 0, m_width, m_height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glm::vec2 spos = screenCoordToBoardCoord(mouseState.m_pos);
    int sx = (int)(spos.x * m_gridSize);
    int sy = (int)(spos.y * m_gridSize);

    int ex = (int)(sx + m_gridSize);
    int ey = (int)(sy + m_gridSize);

  // Utility::debug("grid pos is", glm::vec2(sx, sy));
  //  Utility::debug("grid rightbot is", glm::vec2(ex, sy));

    pipeline temp_pipeline;
    temp_pipeline.loadIdentity();
    temp_pipeline.ortho(-1,1,-1,1,-1,1);

    renderer->enableShader(RENDER_PASS1);
        renderer->setData(RENDER_PASS1, "u_inputTexture", 0,
                                        GL_TEXTURE0,
                                        m_userInputBoardDoubleBuffer.ping.colorTexture);

        renderer->setData(RENDER_PASS1, "u_patternTexture", 1,
                                        GL_TEXTURE1,
                                        pattern->m_patternTexture);

        renderer->setData(RENDER_PASS1, "u_patternBottomRightX", ex);
        renderer->setData(RENDER_PASS1, "u_patternBottomRightY", sy);
        renderer->setData(RENDER_PASS1, "u_patternWidth", pattern->m_width);
        renderer->setData(RENDER_PASS1, "u_patternHeight", pattern->m_height);
        renderer->loadUniformLocations(temp_pipeline, RENDER_PASS1);
        m_boardQuadModel.render();

    renderer->disableShader(RENDER_PASS1);
}


void GameBoard::renderInputToSimulation(Renderer* renderer)
{
    renderIntermediate(renderer, m_inputToSimulationRenderInfo);
}

void GameBoard::renderIntermediate(Renderer* renderer, RenderInfo& rInfo)
{
    glViewport(0, 0, rInfo.width, rInfo.height);
    glBindFramebuffer(GL_FRAMEBUFFER, rInfo.FBOtarget);

    pipeline temp_pipeline;
    temp_pipeline.loadIdentity();
    temp_pipeline.ortho(-1,1,-1,1,-1,1);

    renderer->enableShader(RENDER_PASS1);
        renderer->setData(RENDER_PASS1, "u_sourceTexture", 0, GL_TEXTURE0, rInfo.srcTexture);
        renderer->setData(RENDER_PASS1, "u_srcPresentValue", rInfo.srcPresentValue);
        renderer->setData(RENDER_PASS1, "u_srcEmptyValue", rInfo.srcEmptyValue);
        renderer->setData(RENDER_PASS1, "u_dstPresentValue", rInfo.dstPresentValue);
        renderer->setData(RENDER_PASS1, "u_dstEmptyValue", rInfo.dstEmptyValue);

        renderer->loadUniformLocations(temp_pipeline, RENDER_PASS1);
        m_boardQuadModel.render();

    renderer->disableShader(RENDER_PASS1);
}

void GameBoard::renderSimulation(Renderer* renderer, FBOTargetId target)
{
    if(target == FBO_INPUT)
        renderSimulation(renderer, m_userInputBoardDoubleBuffer.ping.FBO);
    else
        renderSimulation(renderer, 0);
}

void GameBoard::renderSimulation(Renderer* renderer, GLuint fboTarget)
{
    glViewport(0, 0, m_width, m_height);
    glBindFramebuffer(GL_FRAMEBUFFER, fboTarget);

    pipeline temp_pipeline;
    temp_pipeline.loadIdentity();
    temp_pipeline.ortho(-1,1,-1,1,-1,1);

    renderer->enableShader(RENDER_PASS1);
        renderer->setData(RENDER_PASS1, "u_simulationTexture", 0,
                                        GL_TEXTURE0,
                                        m_simulationDoubleBuffer.ping.colorTexture);

        renderer->loadUniformLocations(temp_pipeline, RENDER_PASS1);
        m_boardQuadModel.render();

    renderer->disableShader(RENDER_PASS1);
}

void GameBoard::renderSimulation(Renderer* renderer)
{
    renderSimulation(renderer, 0);
}


void GameBoard::reset()
{
    m_userInputBoardDoubleBuffer.clear(glm::vec4(1.0,1.0,1.0,1.0));
    m_simulationDoubleBuffer.clear();


    m_inputToSimulationRenderInfo.set(m_numGridsX, m_numGridsY,
                                        m_userInputBoardDoubleBuffer.ping.colorTexture,
                                        m_simulationDoubleBuffer.ping.FBO, 0, 1);

    m_simulationToInputRenderInfo.set(m_width, m_height,
                                        m_simulationDoubleBuffer.ping.colorTexture,
                                        m_userInputBoardDoubleBuffer.ping.FBO, 1, 0);
}
