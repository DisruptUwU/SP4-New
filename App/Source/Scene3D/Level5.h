/**
 CScene3D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CKeyboardController
#include "Inputs\KeyboardController.h"
// Include CMouseController
#include "Inputs\MouseController.h"

// Include CFPSCounter
#include "TimeControl\FPSCounter.h"

// Include CSoundController
#include "..\SoundController\SoundController.h"

// Include CSolidObjectManager
#include "Entities\SolidObjectManager.h"
// Include CPlayer3D
#include "Entities\Player3D.h"
// Include CEnemy3D
#include "Entities\Enemy3D.h"
// Include CHealer3D
#include "Entities\Healer3D.h"
// Include CLostSoul3d
#include "Entities\LostSoul3D.h"
// Include CFinalBoss3D
#include "Entities\FinalBoss3D.h"
// Include CFinalNPC
#include "Entities\FinalNPC.h"
// Include CStructure3D
#include "Entities\Structure3D.h"
// Include CProjectileManager
#include "WeaponInfo/ProjectileManager.h"
// Include CSpeed
#include "Entities\Speed.h"
// Include CBearTrap
#include "Entities\BearTrap.h"
// Include CIncreaseDef
#include "Entities\IncreaseDef.h"
// Include CIncreaseDmg
#include "Entities\IncreaseDmg.h"
// Include CDoorLvl5
#include "Entities\DoorLvl5.h"

// Include CCamera
#include "Camera.h"

// Include CSkyBox
#include "SkyBox/SkyBox.h"
// Include CTerrain
#include "Terrain/Terrain.h"

// Include CGUI_Scene3D
#include "GUI_Scene3D.h"

// Include CEntityManager
#include "Entities\EntityManager.h"

class CSettings;

class CLevel5 : public CSingletonTemplate<CLevel5>
{
	friend CSingletonTemplate<CLevel5>;
public:
	// Init
	bool Init(void);

	// Update
	bool Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	void SpawnHealer(int x, int y, int z);
	void SpawnSoul(int x, int y, int z);

	bool sprintCheck = false;
	bool changeDialogue = false;
	bool losegame = false;
	double timer = 10;

	int checkportal = 0;
	bool spawnportal = false;
	bool gotolevel6 = false;

	bool spawnedHealers = false;
protected:
	// The handler to the CSettings
	CSettings* cSettings;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	// Mouse Controller singleton instance
	CMouseController* cMouseController;

	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// FPS Control
	CFPSCounter* cFPSCounter;

	// Handler to the CSoundController
	CSoundController* cSoundController;

	// Handler to the CSolidObjectManager class
	CSolidObjectManager* cSolidObjectManager;
	// Handler to the CProjectileManager instance
	CProjectileManager* cProjectileManager;

	// Handler to the Player3D class
	CPlayer3D* cPlayer3D;
	// Handler to the camera class
	CCamera* cCamera;

	// Handler to the CSkyBox
	CSkyBox* cSkyBox;
	// Handler to the CGround
	CTerrain* cTerrain;

	// GUI_Scene3D
	CGUI_Scene3D* cGUI_Scene3D; //

	// Handler to the EntityManager class
	CEntityManager* cEntityManager;

	double Enddialogtimer = 0;

	// Constructor
	CLevel5(void);
	// Destructor
	virtual ~CLevel5(void);
};

