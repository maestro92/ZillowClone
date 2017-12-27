#include "main.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include "SDL.h"

using namespace std;
using namespace std::placeholders;
// https://www.youtube.com/watch?v=tlXM8qDOS3U
// Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// frame rate
// https://sites.google.com/site/sdlgamer/intemediate/lesson-7
// FPS is 50
const int FRAMES_PER_SECOND = 60;
// So Interval is 1000/50 which is 20ms
// meaning my while loop runs 50 frames per second
// which is every other 20 ms
const int INTERVAL = 1000 / FRAMES_PER_SECOND;

//display surface
SDL_Surface* pDisplaySurface = NULL;
//event structure
SDL_Event event;

static float runningTime = 0.0f;


ExplosionGenerator::ExplosionGenerator()
{
    isRunning = true;
    m_inputMode = true;
    m_switchFlag = false;

    initRenderers();
    initObjects();
    initModels();
    initGUI();

    SDL_WM_SetCaption("GAME OF LIFE", NULL);
}

ExplosionGenerator::~ExplosionGenerator()
{

}

void ExplosionGenerator::initOpenGL()
{
    //Initialize clear color
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    m_pipeline.ortho(-1, 1, -1, 1, 0.5, 1000); //==glOrtho
    m_pipeline.matrixMode(MODEL_MATRIX);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}


void ExplosionGenerator::initRenderers()
{
    m_rm.init();
//    tempTexture = Utility::loadTexture("Assets/Images/tank1B.png");
}


void ExplosionGenerator::initObjects()
{
    m_gridSize = 5;
    m_board = GameBoard(SCREEN_WIDTH-200, SCREEN_HEIGHT, m_gridSize);

    int x = SCREEN_WIDTH - 200;
    int y = 0;
    int w = 200;
    int h = SCREEN_HEIGHT;
    m_gui.init(SCREEN_WIDTH, SCREEN_HEIGHT, x, y, w, h);
}


void ExplosionGenerator::initModels()
{
    m_GOLModelManager.init(m_gridSize);
    m_GOLModelPtr = m_GOLModelManager.getDefaultGOLModel();
}



void ExplosionGenerator::initGUI()
{
    Control::init("", 25, SCREEN_WIDTH, SCREEN_HEIGHT);

    int X_OFFSET = 600;

    int SLIDER_HEIGHT = 35;
    int BUTTON_WIDTH = 200;
    int BUTTON_HEIGHT = 30;

    Control* temp;

    temp = new Label("GAME OF LIFE",
                                X_OFFSET, 0,
                                BUTTON_WIDTH, 120,
                                BLACK);
    temp->setFont(50, WHITE);
    temp->setTextLayout(true, CENTER, CENTER);
    m_gui.addGUIComponent(temp);


    string golDescription = "The Game of Life, also known simply as Life, is a cellular automaton devised by the British mathematician John Horton Conway in 1970.";

    temp = new Label(golDescription,
                        X_OFFSET, 120,
                        BUTTON_WIDTH, 100,
                        BLACK);
    temp->setFont(15, WHITE);
    temp->setTextLayout(true, CENTER, TOP_ALIGNED);
    m_gui.addGUIComponent(temp);



    ListBox* lb = new ListBox("",  X_OFFSET, 220,
                                    200, 400,
                                    WHITE, BLACK, 2,
                                    std::bind(&ExplosionGenerator::GOLModelListBoxCB, this));
    lb->setItemFont(14, GREEN);
    lb->setContent(m_GOLModelManager.getModels());
    lb->setItemsTextLayout(CENTER, CENTER);
    m_gui.addGUIComponent(lb);



    temp = new Button("Start", X_OFFSET, 535,
                                BUTTON_WIDTH, BUTTON_HEIGHT,
                                GRAY, BLACK, DARK_BLUE,
                                std::bind(&ExplosionGenerator::startCB, this));
    temp->setFont(25, GREEN);
    temp->setTextLayout(false, CENTER, CENTER);
    m_gui.addGUIComponent(temp);


    temp = new Button("Reset",  X_OFFSET, 570,
                                BUTTON_WIDTH, BUTTON_HEIGHT,
                                GRAY, BLACK, DARK_BLUE,
                                std::bind(&ExplosionGenerator::resetGameBoardCB, this));
    temp->setFont(25, GREEN);
    temp->setTextLayout(false, CENTER, CENTER);
    m_gui.addGUIComponent(temp);


/*
    temp = new Label("ABCDL_gyp", 0, 100,
                                SCREEN_WIDTH, 25,
                                LIGHT_GRAY);
    temp->setFont(25, BLACK);
    temp->setTextLayout(false, CENTER, CENTER);
    m_gui.addGUIComponent(temp);

    temp = new Label("ABCDL_gyp", 0, 140,
                                SCREEN_WIDTH, 35,
                                LIGHT_GRAY);
    temp->setFont(35, BLACK);
    temp->setTextLayout(false, CENTER, CENTER);
    m_gui.addGUIComponent(temp);

    temp = new Label("ABCDL_gyp", 0, 180,
                                SCREEN_WIDTH, 50,
                                LIGHT_GRAY);
    temp->setFont(50, BLACK);
    temp->setTextLayout(false, CENTER, CENTER);
    m_gui.addGUIComponent(temp);
    */
}


void ExplosionGenerator::start()
{
    cout << "Start" << endl;

    Uint32 startTime = SDL_GetTicks();
    Uint32 next_game_tick = 0;
    Uint32 delay_time = 0;

    while(isRunning)
    {
        startTime = SDL_GetTicks();

        while(SDL_PollEvent(&event))
		{
            int tmpx,tmpy;
            switch(event.type)
			{
                case SDL_QUIT:
                    isRunning = false;
                    break;

                case SDL_MOUSEBUTTONUP:
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            cout << "clicking Up left" << endl;
                            m_mouseState.m_leftButtonDown = false;
                            SDL_GetMouseState(&tmpx,&tmpy);
                            break;

                        case SDL_BUTTON_RIGHT:
                            cout << "clicking Up right" << endl;
                            m_mouseState.m_rightButtonDown = false;
                            SDL_GetMouseState(&tmpx,&tmpy);
                            m_mouseState.m_rightButtonDown = false;

                            break;
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:

                    switch(event.button.button)
                    {
                        int tmpx,tmpy;
                        case SDL_BUTTON_LEFT:
                            cout << "clicking left" << endl;
                            SDL_GetMouseState(&tmpx,&tmpy);
                            m_mouseState.m_leftButtonDown = true;
                            break;

                        case SDL_BUTTON_RIGHT:
                            cout << "clicking right" << endl;
                            SDL_GetMouseState(&tmpx,&tmpy);
                            m_mouseState.m_rightButtonDown = true;
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {

                    }
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            isRunning = false;
                            break;

                        case SDLK_SPACE:
                            if(m_inputMode)
                                m_switchFlag = true;
                            m_inputMode = !m_inputMode;
                            break;
                    }
                    break;
			}
        }
            update();
            forwardRender();
            SDL_GL_SwapBuffers();
            int a = 1;

            if(!m_inputMode)
            {
                next_game_tick += INTERVAL;
                delay_time = next_game_tick - SDL_GetTicks();

                if (next_game_tick > SDL_GetTicks())
                    SDL_Delay(next_game_tick - SDL_GetTicks());
                next_game_tick = SDL_GetTicks() + INTERVAL;
            }


    }
}


void ExplosionGenerator::update()
{
    int mx, my;
    SDL_GetMouseState(&mx,&my);

    m_mouseState.m_pos = glm::vec2(mx, SCREEN_HEIGHT - my);

    if(m_inputMode)
    {
        r_Technique = &m_rm.r_GOLUserInputWithPattern;
        m_board.initUserInput(r_Technique, m_mouseState, m_GOLModelPtr);
        m_board.m_userInputBoardDoubleBuffer.swapFrontBack();
    }
    else
    {
        if(m_switchFlag)
        {
            r_Technique = &m_rm.r_GOLRenderIntermediate;
            m_board.renderInputToSimulation(r_Technique);
            m_switchFlag = false;
        }

        r_Technique = &m_rm.r_GOLUpdate;
        m_board.update(r_Technique);
        m_board.m_simulationDoubleBuffer.swapFrontBack();
    }
}






int main(int argc, char *argv[])
{
    Utility::initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, pDisplaySurface);
    Utility::initGLEW();

    ExplosionGenerator Martin;
    Martin.start();

    Utility::exitSDL(pDisplaySurface);
    //normal termination
    cout << "Terminating normally." << endl;
    return EXIT_SUCCESS;
}





void ExplosionGenerator::forwardRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    if(m_inputMode)
    {
        r_Technique = &m_rm.r_GOLRenderInputWithPattern;
        m_board.renderInput(r_Technique, m_mouseState, m_GOLModelPtr);
    }
    else
    {
        r_Technique = &m_rm.r_GOLRenderSimluation;
        m_board.renderSimulation(r_Technique);
    }

    renderGUI();
}


void ExplosionGenerator::renderGUI()
{

    m_gui.initGUIRenderingSetup();
    /// http://sdl.beuc.net/sdl.wiki/SDL_Average_FPS_Measurement
    unsigned int getTicks = SDL_GetTicks();

//    string final_str = "(" + Utility::floatToStr(m_mouseState.m_pos.x) + ", " + Utility::floatToStr(m_mouseState.m_pos.y) + ")";

    m_gui.updateAndRender(m_mouseState);
}



/// Function CallBacks
void ExplosionGenerator::startCB()
{
    if(m_inputMode)
        m_switchFlag = true;
    m_inputMode = !m_inputMode;
}

void ExplosionGenerator::resetGameBoardCB()
{
    m_inputMode = true;
    m_switchFlag = false;
    m_board.reset();
}

void ExplosionGenerator::GOLModelListBoxCB()
{
    int idx = m_gui.getGOLModelListBoxIndex();
    m_GOLModelPtr = m_GOLModelManager.getModel(idx);
}
