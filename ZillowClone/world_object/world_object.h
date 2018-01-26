#ifndef WORLD_OBJECT_H_
#define	WORLD_OBJECT_H_
#pragma warning(disable: 4996)
#include "utility.h"
#include "renderer.h"
#include "model_enum.h"
#include "model.h"
#include <string>
#include "global.h"

#include "shared.h"
#include <vector>


const glm::vec3 NEG_GRAVITY = glm::vec3(0, -9.8, 0);
const glm::vec3 NEG_HALF_GRAVITY = glm::vec3(0, -4.9, 0);

const glm::vec3 POS_GRAVITY = glm::vec3(0, 9.8, 0);
const glm::vec3 POS_HALF_GRAVITY = glm::vec3(0, 4.9, 0);

using namespace std;


class WorldObject
{
    public:
        WorldObject();

		static WorldObject* getOne();

		virtual ~WorldObject();
		string m_name;

		glm::vec3 m_position;
		glm::vec3 m_velocity;
		glm::vec3 m_scale;
		glm::mat4 m_rotation;
		glm::mat4 m_modelMatrix;


		ObjectId objectId;

		bool active;

		Model* m_model;


		inline void setName(string s);
		inline string getName();


        inline void setScale(float s);
        inline void setScale(glm::vec3 scale);
        inline void setScale(float x, float y, float z);

        inline void setPosition(glm::vec3 pos);
        inline void setPosition(float x, float y, float z);

        inline void setVelocity(glm::vec3 vel);
        inline void setVelocity(float x, float y, float z);

		inline void setModel(Model* model);

		inline void updateModelMatrix();

		inline glm::vec3 getPosition();
		inline glm::vec3 getVelocity();
		inline glm::vec3 getScale();
		inline glm::mat4 getRotation();

		glm::vec3 m_xAxis;
		glm::vec3 m_yAxis;
		glm::vec3 m_zAxis;
        inline void setRotation(glm::mat4 rot);

		bool shouldRender();

		void renderSingle(Pipeline& p, Renderer* r);
		void renderGroup(Pipeline& p, Renderer* r);


		void print_uint8_t(uint8_t n);


};


inline void WorldObject::setName(string s)
{
	m_name = s;
}

inline string WorldObject::getName()
{
	return m_name;
}

inline void WorldObject::setPosition(glm::vec3 pos)
{
    m_position = pos;
}

inline void WorldObject::setPosition(float x, float y, float z)
{
    m_position = glm::vec3(x, y, z);
}

inline void WorldObject::setScale(float s)
{
	m_scale = glm::vec3(s, s, s);
}

inline void WorldObject::setScale(glm::vec3 scale)
{
	m_scale = scale;
}

inline void WorldObject::setScale(float x, float y, float z)
{
	m_scale = glm::vec3(x, y, z);
}

inline void WorldObject::setRotation(glm::mat4 rot)
{
	m_xAxis = glm::vec3(rot[0][0], rot[0][1], rot[0][2]);
	m_yAxis = glm::vec3(rot[1][0], rot[1][1], rot[1][2]);
	m_zAxis = glm::vec3(rot[2][0], rot[2][1], rot[2][2]);

	float temp[16] = {rot[0][0], rot[0][1], rot[0][2], 0.0,
                      rot[1][0], rot[1][1], rot[1][2], 0.0,
                      rot[2][0], rot[2][1], rot[2][2], 0.0,
                      0.0,       0.0,       0.0,       1.0};
    m_rotation = glm::make_mat4(temp);
}





inline void WorldObject::setVelocity(glm::vec3 vel)
{
	m_velocity = vel;
}

inline void WorldObject::setVelocity(float x, float y, float z)
{
	m_velocity = glm::vec3(x, y, z);
}


inline glm::vec3 WorldObject::getPosition()
{
	return m_position;
}

inline glm::vec3 WorldObject::getVelocity()
{
	return m_velocity;
}

inline glm::vec3 WorldObject::getScale()
{
	return m_scale;
}

inline glm::mat4 WorldObject::getRotation()
{
	return m_rotation;
}


inline void WorldObject::setModel(Model* model)
{
	m_model = model;
}



inline void WorldObject::updateModelMatrix()
{
	m_modelMatrix = glm::translate(m_position) * m_rotation * glm::scale(m_scale);
}

#endif
