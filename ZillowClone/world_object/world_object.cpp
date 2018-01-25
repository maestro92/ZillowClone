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

	m_mass = 1.0;
	m_invMass = 1.0 / m_mass;
	
	m_model = NULL;

	isTested = isCollided = isHit = false;
	isHitCounter = 0;

	setMaterialEnergyRestitution(0.0f);
	setMaterialSurfaceFriction(1);

	m_entityType = SCENE_OBJECT;
	m_dynamicType = STATIC;

	m_modelEnum = -1;
}


WorldObject* WorldObject::getOne()
{
	WorldObject* obj = new WorldObject();
	return obj;
}

bool WorldObject::hasOwner()
{
	return ownerId != ObjectId::NO_OWNER;
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


EntityType WorldObject::getEntityType()
{
	return m_entityType;
}

DynamicType WorldObject::getDynamicType()
{
	return m_dynamicType;
}


void WorldObject::updateGameInfo()
{
	if (isHit)
	{
		isHitCounter++;
	}

	if (isHitCounter >= 20)
	{
		isHit = false;
		isHitCounter = 0;
	}

}

bool WorldObject::isPlayer()
{
	return m_entityType == PLAYER;
}

bool WorldObject::isWeapon()
{
	return m_entityType == WEAPON;
}

void WorldObject::setHP(int hp)
{
	m_curHP = hp;
}
void WorldObject::setArmor(int armor)
{
	m_curArmor = armor;
}


void WorldObject::takeDamage(int damage)
{
	if (isPlayer())
	{
		// check armor, if we have armor, deduct from armor		
		if (m_curArmor >= 0)
		{
			m_curArmor -= damage;
			if (m_curArmor < 0)
			{
				damage = -m_curArmor;
				m_curArmor = 0;
			}
			else
			{
				damage = 0;
			}
		}

		// deduct HP if there is carry over
		m_curHP -= damage;
		m_curHP = std::max(0, m_curHP);
		if (m_curHP <= 0)
		{
			// i'm dead
			cout << "Player " << objectId.getIndex() << " dead" << endl;
		}
	}
}


bool WorldObject::isDead()
{
	return m_curHP <= 0;
}

void WorldObject::print_uint8_t(uint8_t n)
{
	int i;
	for (i = 8; i >= 0; i--)
		printf("%d", (n & (1 << i)) >> i);
	putchar('\n');
}


void WorldObject::updateContactNormalInfo(glm::vec3 normal)
{

}


bool WorldObject::shouldRender()
{
	return true;
}



 bool WorldObject::shouldSend(int clientId)
{
	if (isWeapon() && hasOwner() && ownerId.getIndex() == clientId)
	{
		return false;
	}
	else
	{
		return true;
	}
}
 
