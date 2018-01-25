#ifndef MODEL_MANAGER_H_
#define MODEL_MANAGER_H_

using namespace std;

#include <unordered_map>
#include "model_enum.h"

#include "utility.h"

#include "model.h"
#include "quad_model.h"
#include "xyz_axis_model.h"
#include "bezier_points.h"


#include "json_spirit.h"
#include "json_spirit_reader_template.h"
#include "json_spirit_writer_template.h"
#include <cassert>
#include <fstream>

#ifndef JSON_SPIRIT_MVALUE_ENABLED
#error Please define JSON_SPIRIT_MVALUE_ENABLED for the mValue type to be enabled 
#endif

using namespace std;
using namespace json_spirit;

// to resolve circular depenency issue
// class Weapon;

class ModelManager
{


	public:

		ModelManager();
		~ModelManager();

		void init();
		void shutDown();

		Model* get(int modelEnum);

	private:
		Model* m_xyzAxis;
		Model* m_bezierPoints;

		vector<Model*> m_models;
};



#endif