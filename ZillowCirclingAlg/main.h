#ifndef MAIN_H_
#define MAIN_H_

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include "define.h"

#include "gol_model_manager.h"
#include "game_board.h"

#include "utility.h"
#include "shader.h"


#include "label.h"


#include "gui_manager.h"
#include "slider.h"

#include "pipeline.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "renderer_manager.h"
#include "renderer.h"


#define NO_SDL_GLEXT
#include <GL/glew.h>
#include "SDL_opengl.h"

#include "SDL.h"
#include "SDL_image.h"

#include "text_engine.h"


#define FRAME_VALUES 10

using namespace std;

/// For style
/// use http://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Function_Names


/// C++ style
/// http://geosoft.no/development/cppstyle.html


/// http://stackoverflow.com/questions/1228161/why-use-prefixes-on-member-variables-in-c-classes
///
/*
I use:

m for members
c for constants/readonlys
p for pointer (and pp for pointer to pointer)
v for volatile
s for static
i for indexes and iterators
e for events
*/
// http://stackoverflow.com/questions/4845410/error-lnk2019-unresolved-external-symbol-main-referenced-in-function-tmainc
#undef main

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

class ExplosionGenerator
{
    private:
        RendererManager              m_rm;
        Renderer*                    r_Technique;


        /// GUI
        long long m_runningTime;
        MouseState m_mouseState;

        pipeline m_pipeline;

        bool m_inputMode;
        bool m_switchFlag;



   //     EG_Model*   modelPtr;
        EG_ModelABS*    p_modelPtr;


        float m_fps;
        float m_iterRefreshRate;
        float m_curIter;
        unsigned int m_frameCount;
        unsigned int m_frameTicks[FRAME_VALUES];
        unsigned int m_frameTicksIndex;
        unsigned int m_prevFrameTick;

        bool isRunning;
        GOLModel* m_GOLModelPtr;

        int m_gridSize;

        GLuint tempTexture;

        GameBoard m_board;

        GUIManager m_gui;
        GOLModelManager m_GOLModelManager;
    public:

        ExplosionGenerator();
        ~ExplosionGenerator();

        /// init functions
        void initOpenGL();
        void initObjects();
        void initModels();
        void initRenderers();
        void initGUI();

        void start();
        void update();

        void forwardRender();
        void renderScene();

        void renderGUI();


        void startCB();
        void resetGameBoardCB();
        void GOLModelListBoxCB();
};




#endif
