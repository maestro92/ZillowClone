#ifndef XYZ_AXIS_MODEL_H_
#define XYZ_AXIS_MODEL_H_

#include <iostream>
#include <cstdlib>

#include "pipeline.h"
#include "EG_ModelABS.h"

using namespace std;

class XYZAxisModel : public EG_ModelABS
// class EG_WorldAxis
{
    public:
        // Constructor
        XYZAxisModel();
		~XYZAxisModel();


        void init();
//        void render(pipeline &m_pipeline, EG_RenderTechnique* RenderTechnique, int RenderPassID);
   //     void buildAxisMesh();
};


/*
class EG_WorldAxis : public WorldObject
// class EG_WorldAxis
{
    public:
        // Constructor
        EG_WorldAxis();
		~EG_WorldAxis();

        mesh* worldAxis;

        void init();
        void renderSingle(pipeline &m_pipeline, EG_RenderTechnique* RenderTechnique, int RenderPassID);
        void render(pipeline &m_pipeline, EG_RenderTechnique* RenderTechnique, int RenderPassID);
//        void render(pipeline &m_pipeline, EG_RenderTechnique* RenderTechnique, int RenderPassID);
   //     void buildAxisMesh();
};
*/

#endif
