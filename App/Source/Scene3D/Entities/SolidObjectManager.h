#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

// Include CSolidObject
#include "SolidObject.h"

// Include CProjectileManager
#include "../WeaponInfo/ProjectileManager.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "Hydra.h"
#include "FinalBoss3D.h"
#include "FinalNPC.h"
#include "Player3D.h"
#include "../App/Demon.h"

// Include list
#include <list>

class CSolidObjectManager : public CSingletonTemplate<CSolidObjectManager>
{
	friend class CSingletonTemplate<CSolidObjectManager>;

public:
	// Initialise this class instance
	bool Init(void);

	// Set model
	virtual void SetModel(const glm::mat4 model);
	// Set view
	virtual void SetView(const glm::mat4 view);
	// Set projection
	virtual void SetProjection(const glm::mat4 projection);

	// Add a CSolidObject* to this class instance
	virtual void Add(CSolidObject* cSolidObject);
	// Remove a CSolidObject* from this class instance
	virtual bool Erase(CSolidObject* cSolidObject);

	// Collision Check for a CSolidObject*
	virtual bool CollisionCheck(CSolidObject* cSolidObject);

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// Collision Check for all entities and projectiles
	virtual bool CheckForCollision(void);

	// CleanUp all CSolidObjects which are marked for deletion
	virtual void CleanUp(void);

	// Render this class instance
	virtual void Render(void);

	bool HydraKilled = false;

	bool FinalBossKilled = false;

	bool wenttodoor = false;

	bool youlose = false;

	bool healthbelow50 = false;

	bool DemonKilled = false;

	//bool hydrashotplayer = false;
	bool moreaggresivepart2 = false;

	CHydra* cHydra;

	CFinalBoss3D* cFinalBoss3D;

	CFinalNPC* cFinalNPC;

	CDemon* cDemon;

	CPlayer3D* cPlayer3D;

	// Hardcode int value fpr keeping track of lvl 1 eneimes
	int enemy_lvl1_count;
protected:
	// Render Settings
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	// List of CSolidObject
	std::list<CSolidObject*> lSolidObject;

	// Handler to the CProjectileManager
	CProjectileManager* cProjectileManager;

	// Default Constructor
	CSolidObjectManager(void);
	// Destructor
	virtual ~CSolidObjectManager(void);
};
