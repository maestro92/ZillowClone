#ifndef GAME_BOARD_H_
#define GAME_BOARD_H_


#include "define.h"
#include "quad_model.h"
#include "renderer.h"
#include "gol_model.h"

using namespace std;

enum BufferId
{
    FRONT = 0,
    BACK
};

enum FBOTargetId
{
    FBO_INPUT = 0,
    FBO_SIMULATION
};

class GameBoard
{
    private:
        struct RenderInfo
        {
            int width;
            int height;
            GLuint srcTexture;
            GLuint FBOtarget;
            int srcPresentValue;
            int srcEmptyValue;
            int dstPresentValue;
            int dstEmptyValue;

            RenderInfo()
            { }

            void set(int w, int h, GLuint srcTx, GLuint fbo, int rpv, int wpv)
            {
                width = w;
                height = h;

                srcTexture = srcTx;
                FBOtarget = fbo;

                srcPresentValue = rpv;
                srcEmptyValue = 1 - rpv;

                dstPresentValue = wpv;
                dstEmptyValue = 1 - wpv;
            }
        };

        int m_numGridsX;
        int m_numGridsY;
        float m_width;
        float m_height;
        float m_inverseWidth;
        float m_inverseHeight;

        int m_gridSize;
        float m_invGridSize;

        RenderInfo m_inputToSimulationRenderInfo;
        RenderInfo m_simulationToInputRenderInfo;

        QuadModel m_boardQuadModel;


    public:
        DoubleFrameBufferObject m_userInputBoardDoubleBuffer;
        DoubleFrameBufferObject m_simulationDoubleBuffer;

        GameBoard() : GameBoard(20, 20, 1)
        {}

        GameBoard(int w, int h, int s);

        int getWidth();
        int getHeight();
        int getGridSize();

        void setWidth(int w);
        void setHeight(int h);
        void setGridSize(int s);

        bool getBoard(int x, int y);


        void inputToSimluationBoard(Renderer* renderer);
        void initUserInput(Renderer* renderer, MouseState& mouseState, GOLModel* pattern);
        void update(Renderer* renderer);

        void renderInput(Renderer* renderer, MouseState& mouseState, GOLModel* pattern);
        void renderInputToSimulation(Renderer* renderer);
        void renderSimulationToInput(Renderer* renderer);
        void renderIntermediate(Renderer* renderer, RenderInfo& rInfo);

        void renderSimulation(Renderer* renderer, FBOTargetId target);
        void renderSimulation(Renderer* renderer, GLuint fboTarget);
        void renderSimulation(Renderer* renderer);

        inline glm::vec2 screenCoordToBoardCoord(glm::vec2 pos)
        {
            glm::vec2 bCoord;
            bCoord.x = floor(pos.x * m_invGridSize);
            bCoord.y = floor(pos.y * m_invGridSize);
            return bCoord;
        }

        void reset();
};


#endif
