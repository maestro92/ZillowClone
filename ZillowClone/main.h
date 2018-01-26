#ifndef MAIN_H_
#define MAIN_H_

class WorldObject;

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <unordered_set>
#include "SDL.h"
#include "SDL_image.h"

#include "define.h"
#include "utility.h"

#include <queue>

#include <GL/glew.h>

#include "utility.h"
#include "shader.h"

#include "label.h"
#include "gui_manager.h"
#include <chrono>

#include "pipeline.h"

#include <ft2build.h>

#include "renderer_manager.h"
#include "renderer.h"
#include "renderer_constants.h"


#include "quad_model.h"
#include "xyz_axis_model.h"
#include "world_object.h"

#include "model_manager.h"

#define FRAME_VALUES 10
#include <list>

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#include <vector>

using namespace std;
/*

For style
use http://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Function_Names

C++ style
http://geosoft.no/development/cppstyle.html

http://stackoverflow.com/questions/1228161/why-use-prefixes-on-member-variables-in-c-classes

I use:

m for members
c for constants/readonlys
p for pointer (and pp for pointer to pointer)
v for volatile
s for static
i for indexes and iterators
e for events
*/



// this class assumes T is pointer
template <class T>
struct FOArray
{
	struct Entry
	{
		ObjectId objId;
		T object;

		Entry()
		{
			object = NULL;
		}
	};


	vector<Entry> objects;
	vector<int> freeList;

	vector<T> toBeDestroyed;


	// i'm using this to start iterating for situations where I want to skip the static objects
	// i am making the assumption that all my static objects are put in my array first
	int preferredIterationStart;
	int maxSize;
	int maxUsed;
	int count;

	// getting only the number of dynamic objects;
	int getPreferredCount()
	{
		return maxSize - preferredIterationStart;
	}

	void init(int maxSize)
	{
		this->maxSize = maxSize;
		objects.resize(maxSize);

		for (int i = maxSize - 1; i >= 0; i--)
		{
			freeList.push_back(i);
		}

		for (int i = 0; i < maxSize; i++)
		{
			objects[i].objId.setTag(0);
			objects[i].objId.setIndex(i);
		}

		maxUsed = 0;
		count = 0;
	}

	T getByIndex(int index)
	{
		return objects[index].object;
	}

	T get(ObjectId objId)
	{
		//		return objects[id];
		//		return objects[id]->indexId == -1 ? NULL : objects[id];
		return objects[objId.getIndex()].object;
	}

	void clearFreeList()
	{
		freeList.clear();
	}

	ObjectId create()
	{
		if (!freeList.empty())
		{
			int free = freeList.back();
			freeList.pop_back();

			ObjectId objId = objects[free].objId;
			// the new version + index is managed by destroyedObject, so you don't have to
			// change the id here.
			objects[free].object = new WorldObject();
			objects[free].object->objectId = objId;

			count++;
			maxUsed = max(maxUsed, free);

			return objId;
		}
		else
		{
			ObjectId objId;
			objId.setId(INVALID_OBJECT_ID);
			return objId;
		}
	}


	ObjectId add(T obj)
	{
		if (!freeList.empty())
		{
			int free = freeList.back();
			freeList.pop_back();

			ObjectId objId = objects[free].objId;



			// the new version + index is managed by destroyedObject, so you don't have to
			// change the id here.
			objects[free].object = obj;
			objects[free].object->objectId = objId;

		//	cout << obj->m_name << "	tag is " << obj->objectId.getTag() << endl;
		//	cout << obj->m_name << "	index is " << obj->objectId.getIndex() << endl;

			count++;
			maxUsed = max(maxUsed, free);

			return objId;
		}
		else
		{
			ObjectId objId(ObjectId::INVALID_OBJECT_ID);
			return objId;
		}
	}

	void addToDestroyList(ObjectId objId)
	{
		toBeDestroyed.push_back(get(objId));
	}

	void destroyObjects()
	{
		for (int i = 0; i < toBeDestroyed.size(); i++)
		{
			WorldObject* obj = toBeDestroyed[i];
			destroy(obj->objectId);
		}

		toBeDestroyed.clear();
	}

	// TODO: need to fix this, de-allocation is expensive
	void destroy(ObjectId objId)
	{
		delete get(objId);

		int tag = objId.getTag();
		int index = objId.getIndex();

		objects[index].objId.setTag(tag + 1);
		objects[index].object = NULL;

		count--;

		freeList.push_back(index);
	}

	//	ObjectId nextAvaiableIndexId()
	ObjectId nextAvailableId()
	{
		if (!freeList.empty())
		{
			int free = freeList.back();
			return objects[free].objId;
		}
		else
		{
			ObjectId objId(ObjectId::INVALID_OBJECT_ID);
			return objId;
		}
	}

	// only ever used on the client side
	// do not use this on the server side
	// becuz the client objects list is only ever changed by snapshot updates from the server
	// therefore we do not have to worry about freeList
	void set(T obj, ObjectId objId)
	{
		int index = objId.getIndex();

		//		WorldObject* obj1 = objects[index].object;
		// cout << "		## index is " << index << endl;
		// cout << "		## new object is " << obj->m_name << endl;

		if (objects[index].object != NULL)
		{
			//	cout << "		## i want to delete " << (objects[index].object->m_name) << endl;
			delete objects[index].object;
		}
		else
		{
			// should remove index from freeList
			// but this is only used by the client
			// so we don't have to care about that
			count++;
		}

		objects[index].objId = objId;
		objects[index].object = obj;

		maxUsed = max(maxUsed, index);
	}

	int getIterationEnd()
	{
		return maxUsed + 1;
	}

	void debugIds()
	{
		for (int i = 0; i < maxUsed; i++)
		{
			cout << i << endl;
			T* obj = objects[i].object;
			if (obj != NULL)
			{
				cout << obj->m_name << "	tag is " << obj->objectId.getTag() << endl;
				cout << obj->m_name << "	index is " << obj->objectId.getIndex() << endl;
			}
			cout << endl;
		}

	}

};




const int TIME_PROFILER_BUFFER = 10;
const int FPS_PROFILER_BUFFER = 20;



class ZillowClone
{
	public:
		Renderer*					p_renderer;
		
		/// GUI
		long long m_runningTime;
		Uint32 m_nextGameTick = 0;
		MouseState m_mouseState;

		Pipeline m_pipeline;

		float m_fps;
		float m_iterRefreshRate;
		float m_curIter;
		unsigned int m_frameCount;
		unsigned int m_frameTicks[FRAME_VALUES];
		unsigned int m_frameTicksIndex;
		unsigned int m_prevFrameTick;

		bool isRunning;


		// models
		Model*          p_model;


		bool containedFlag;

		WorldObject     o_worldAxis;
		WorldObject		o_bezierPoint;


		int debugCurClientId;


		GUIManager m_gui;

		float m_zoom;
		float m_range;
	public:

		long long m_currentTimeMillis;

		void updateZoom(bool zoomingIn);

		int timeProfilerIndex;
		long long timeProfiler[TIME_PROFILER_BUFFER];

		int fpsProfilerIndex;
		int fpsProfiler[FPS_PROFILER_BUFFER];

		
		FOArray<WorldObject*> sv_objects;
		FOArray<WorldObject*> cl_objects;	// this never creates, only sets objects



		ZillowClone();
		ZillowClone(int nice);
		~ZillowClone();

		/// init functions
		void init();
		void initObjects();

		void initGUI();

		
		int endWithError(char* msg, int error = 0);

		void serverHandleDeviceEvents();
		void clientHandleDeviceEvents();

		void onMouseBtnUp();
		void onMouseBtnDown();

		void start();
		void update();
		bool inDrawingMode;
		bool startedCurrentLine;


		int getAverageFPS();

		void clientFrame(long long dt);


		bool isNewPoint(glm::vec2 newPoint);


		void render();


		void GetTimeProfilerAverages();



		void renderGUI();



		vector<int> latencyOptions;		// round trip
		int latency;					// rount trip latency in milliseconds
		int curLatencyOption;

		glm::vec2 lastPoint;
		vector<glm::vec2> points;
		vector<WorldObject> pointRenderHandles;

		long long getCurrentTimeMillis();
};

#endif