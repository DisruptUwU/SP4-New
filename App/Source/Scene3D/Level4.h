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
// Include CDragon
#include "Entities\Dragon.h"
// Include CDoor
#include "Entities/door.h"
// Include CDoor
#include "Entities/Healthup.h"
// Include CDoor
#include "Entities/IncreaseDmg.h"
// Include CStructure3D
#include "Entities\Structure3D.h"
// Include CProjectileManager
#include "WeaponInfo/ProjectileManager.h"

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

class CLevel4 : public CSingletonTemplate<CLevel4>
{
	friend CSingletonTemplate<CLevel4>;
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

	bool sprintCheck = false;

	bool losegame = false;

	bool bNextLevel;
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
	CGUI_Scene3D* cGUI_Scene3D;

	// Handler to the EntityManager class
	CEntityManager* cEntityManager;

	CDoor* cDoor;

	int checkfirstroar = 0;
	int checklastroar = 0;
	int checkroar1 = 0;
	int checkroar2 = 0;
	int checkroar3 = 0;
	int checkroar4 = 0;
	int checkplayerdie = 0;
	double gametimer = 0;
	bool playSounds = true;
	bool spawnpower1 = false;
	bool spawnpower2 = false;

	// Constructor
	CLevel4(void);
	// Destructor
	virtual ~CLevel4(void);
};

