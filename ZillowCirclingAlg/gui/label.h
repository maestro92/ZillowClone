#ifndef EG_LABEL_H
#define EG_LABEL_H

#include "pipeline.h"
#include "define.h"
#include "utility.h"
//#include "EG_Quad.h"


#define NO_SDL_GLEXT
#include <GL/glew.h>
#include "SDL_opengl.h"
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>

#include "Control.h"

using namespace std;


class Label : public Control
{
    public:

        Label();
        Label(string text, int x, int y, int width, int height, glm::vec3 color);
        virtual int getType();
        void render();
        void customRender();
};



#endif
