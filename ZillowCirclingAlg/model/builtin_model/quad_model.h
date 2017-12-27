#ifndef QUAD_MODEL_H_
#define QUAD_MODEL_H_


#include "EG_ModelABS.h"

using namespace std;

class QuadModel : public EG_ModelABS
{
    public:
        // constructor
		/*
		QuadModel();
		QuadModel(int w, int h);

		QuadModel(int w, int h,
			float uv_x, float uv_y, float uv_w);

		QuadModel(float l, float r,
			float b, float t);

		QuadModel(float l, float r,
			float b, float t,
			float uv_x, float uv_y, float uv_w);

		// Destructor
		~QuadModel();

		/// textures
		void initIndices(vector<unsigned int>& indices);
		void init(float l, float r,
			float b, float t,
			float cx, float cy, float cw);
			*/
		
        QuadModel();
        QuadModel(int i);

        QuadModel(int w, int h);
        QuadModel(int w, int h, float uv_x, float uv_y, float uv_w);

        // Destructor
		~QuadModel();

        /// textures
        void initIndices(vector<unsigned int>& indices);
        void init();
        void init(int w, int h);
        void init(int w, int h, float cx, float cy, float cw);


        /// color
        void init(int w, int h, glm::vec3 c);
        void init(int w, int h, glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4);

        void initCentered();

        void init(float l, float r,
                  float b, float t,
                  float cx, float cy, float cw);
				  
};


#endif
