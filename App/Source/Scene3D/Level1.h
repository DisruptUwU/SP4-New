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

// Include CChest
#include "Entities\Chest.h"
// Include CChestZone
#include "Entities\Chest_Zone.h"

// Include CSolidObjectManager
#include "Entities\SolidObjectManager.h"
// Include CPlayer3D
#include "Entities\Player3D.h"
/*// Include CEnemy3D
#include "Entities\Enemy3D.h"*/
// Include CEnemy3D from lvl 2
#include "Entities\Enemylvl2.h"
// Include CHealthup
#include "Entities\Healthup.h"
// Include CDoor
#include "Entities\door.h"
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

class CLevel1 : public CSingletonTemplate<CLevel1>
{
	friend CSingletonTemplate<CLevel1>;
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

	bool teleport_2_lvl2 = false;
		
	int checkplayerdie = 0;

	bool losegame = false;

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

	// Background music bool
	bool background_music_lvl1;

	// Constructor
	CLevel1(void);
	// Destructor
	virtual ~CLevel1(void);
};

