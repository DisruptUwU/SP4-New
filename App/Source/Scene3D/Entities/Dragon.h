#pragma once

#include "Enemy3D.h"

class CDragon : public CEnemy3D
{
public:
	CDragon(glm::vec3 position = glm::vec3());
	~CDragon();
};