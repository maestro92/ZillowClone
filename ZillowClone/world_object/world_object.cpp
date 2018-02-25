#include "world_object.h"
#include "model_manager.h"

WorldObject::WorldObject()
{
	active = true;
	// m_instanceId = utl::createUniqueObjectID();

    m_position = glm::vec3(0.0, 0.0, 0.0);
    m_velocity = glm::vec3(0.0, 0.0, 0.0);
    m_scale = glm::vec3(1.0, 1.0, 1.0);
    m_rotation = glm::mat4(1.0);
	
	m_model = NULL;
}


WorldObject* WorldObject::getOne()
{
	WorldObject* obj = new WorldObject();
	return obj;
}



WorldObject::~WorldObject()
{

}

void WorldObject::renderSingle(Pipeline& p, Renderer* r)
{
	r->enableShader();
		renderGroup(p, r);
	r->disableShader();
}

void WorldObject::renderGroup(Pipeline& p, Renderer* r)
{
	p.pushMatrix();
		p.translate(m_position);
		p.addMatrix(m_rotation);
		p.scale(m_scale);
		r->setUniLocs(p);
		m_model->render();
	p.popMatrix();
}




void WorldObject::print_uint8_t(uint8_t n)
{
	int i;
	for (i = 8; i >= 0; i--)
		printf("%d", (n & (1 << i)) >> i);
	putchar('\n');
}

bool WorldObject::canRender()
{
	return m_model != NULL;
}



bool WorldObject::shouldRender()
{
	return true;
}

