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
#include "Entities\Enemylvl2.h"
// Include CStructure3D
#include "Entities\Structure3D.h"
#include "Entities\Coin.h"
#include "Entities\Speed.h"
#include "Entities\Healthup.h"
// Include CStructure3D
#include "Entities\doorlvl2.h"
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

class CLevel2 : public CSingletonTemplate<CLevel2>
{
	friend CSingletonTemplate<CLevel2>;
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
	bool wavedead = false;
	bool notdead = true;
	bool status;
	bool gotolevel3 = false;
	bool loseGame = false;

	double gametimer = 0;

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

	// Constructor
	CLevel2(void);
	// Destructor
	virtual ~CLevel2(void);

	int checkplayerdie = 0;
	int checkDemonspawn = 0;
	int checkSound1 = 0;
	int checkSound2 = 0;
	int checkSound3 = 0;
	int checkSound4 = 0;
	bool playSounds = true;
	bool Demonspawn = false;
	int portalcheck = 0;
	bool portalspawn;
	bool spawnpower1 = false;
	bool spawnpower2 = false;
	double demontexttimer = 3;
};

