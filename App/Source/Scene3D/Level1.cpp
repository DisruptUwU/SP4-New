/**
 CScene3D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Level1.h"

// Include GLEW
#ifndef GLEW_STATIC
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif

// Include this for glm::to_string() function
#define GLM_ENABLE_EXPERIMENTAL
#include <includes/gtx/string_cast.hpp>

// Include filesystem to read from a file
#include "System\filesystem.h"

// Include CShaderManager
#include "RenderControl/ShaderManager.h"

// Include CPistol
#include "WeaponInfo\Pistol.h"

// Include CCameraShake
#include "CameraEffects/CameraShake.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CLevel1::CLevel1(void)
	: cSettings(NULL)
	, cKeyboardController(NULL)
	, cMouseController(NULL)
	, cFPSCounter(NULL)
	, cGUI_Scene3D(NULL)
	, cSoundController(NULL)
	, cSolidObjectManager(NULL)
	, cPlayer3D(NULL)
	, cCamera(NULL)
	, cSkyBox(NULL)
	, cTerrain(NULL)
	, teleport_2_lvl2(false)
	, background_music_lvl1(true)
{
}

/**
 @brief Destructor
 */
CLevel1::~CLevel1(void)
{
	// Destroy the cTerrain
	if (cTerrain)
	{
		cTerrain->Destroy();
		cTerrain = NULL;
	}

	// Destroy the cSkyBox
	if (cSkyBox)
	{
		cSkyBox->Destroy();
		cSkyBox = NULL;
	}

	// Destroy the camera
	if (cCamera)
	{
		cCamera->Destroy();
		cCamera = NULL;
	}

	// Destroy the cEntityManager
	if (cEntityManager)
	{
		cEntityManager->Destroy();
		cEntityManager = NULL;
	}

	// Destroy the cProjectileManager
	if (cProjectileManager)
	{
		cProjectileManager->Destroy();
		cProjectileManager = NULL;
	}

	// Destroy the cPlayer3D
	if (cPlayer3D)
	{
		cSolidObjectManager->Erase(cPlayer3D);
		cPlayer3D->Destroy();
		cPlayer3D = NULL;
	}

	// Destroy the cSolidObjectManager
	if (cSolidObjectManager)
	{
		cSolidObjectManager->Destroy();
		cSolidObjectManager = NULL;
	}
	// Destroy the cSoundController
	if (cSoundController)
	{
		cSoundController->Destroy();
		cSoundController = NULL;
	}

	// Destroy the cSoundController
	if (cGUI_Scene3D)
	{
		cGUI_Scene3D->Destroy();
		cGUI_Scene3D = NULL;
	}

	// We won't delete this since it was created elsewhere
	cFPSCounter = NULL;

	// We won't delete this since it was created elsewhere
	cMouseController = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;
	
	// We won't delete this since it was created elsewhere
	cSettings = NULL;
}

/**
 @brief Init Initialise this instance
 @return true if the initialisation is successful, else false
 */ 
bool CLevel1::Init(void)
{
	cSettings = CSettings::GetInstance();

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Configure the camera
	cCamera = CCamera::GetInstance();
	cCamera->vec3Position = glm::vec3(0.0f, 0.5f, 3.0f);

	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();

	// Store the mouse controller singleton instance here
	cMouseController = CMouseController::GetInstance();

	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();

	// Load the sounds into CSoundController
	cSoundController = CSoundController::GetInstance();
	cSoundController->Init();
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_Bell.ogg"), 1, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_Explosion.ogg"), 2, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Sound_Jump.ogg"), 3, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\heartbeat.ogg"), 4, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Jumpscarehydra.ogg"), 5, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\lvl1_background.ogg"), 6, true);

	// Load the Environment Entities
	// Load the SkyBox
	cSkyBox = CSkyBox::GetInstance();
	// Set a shader to this class instance of CSkyBox
	cSkyBox->SetShader("Shader3D_SkyBox");
	cSkyBox->Init();

	// Load the Ground
	cTerrain = CTerrain::GetInstance();
	cTerrain->SetShader("Shader3D_Terrain");
	cTerrain->InitLevel1();
	// Set the size of the Terrain
	cTerrain->SetRenderSize(200.0f, 5.0f, 200.0f);

	// Load the movable Entities
	// Initialise the CSolidObjectManager
	cSolidObjectManager = CSolidObjectManager::GetInstance();
	cSolidObjectManager->Init();

	// Initialise the cPlayer3D
	cPlayer3D = CPlayer3D::GetInstance();
	cPlayer3D->SetPosition(glm::vec3(0.0f, 0.5f, 40.0f));
	cPlayer3D->SetShader("Shader3D");
	cPlayer3D->Init();
	cPlayer3D->InitCollider("Shader3D_Line", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	cPlayer3D->AttachCamera(cCamera);

	// Add the cPlayer3D to the cSolidObjectManager
	cSolidObjectManager->Add(cPlayer3D);

	cPlayer3D->at_level1 = true;

	// Initialise the projectile manager
	cProjectileManager = CProjectileManager::GetInstance();
	cProjectileManager->Init();
	cProjectileManager->SetShader("Shader3D");

	// Assign a cPistol to the cPlayer3D
	CPistol* cPistol = new CPistol();
	// Set the position, rotation and scale of this weapon
	cPistol->SetPosition(glm::vec3(0.05f, -0.075f, -0.3f));
	cPistol->SetRotation(3.14159f, glm::vec3(0.0f, 1.0f, 0.0f));
	cPistol->SetScale(glm::vec3(0.75f, 0.75f, 0.75f));
	// Initialise the instance
	cPistol->Init();
	cPistol->SetShader("Shader3D_Model");
	cPlayer3D->SetWeapon(0, cPistol);



	float fCheckHeight;

	/*// Initialise the health powerup
	fCheckHeight = cTerrain->GetHeight(0.0f, 0.0f);
	CHealthup* cHealthup = new CHealthup(glm::vec3(0.0f, fCheckHeight, 0.0f));
	cHealthup->SetShader("Shader3D");
	cHealthup->Init();
	cHealthup->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	// Add the health pickup to the cSolidObjectManager
	cSolidObjectManager->Add(cHealthup);*/

	// Initialise the chest 
	fCheckHeight = cTerrain->GetHeight(0.0f, 0.0f);
	CChest* cChest = new CChest(glm::vec3(0.0f, fCheckHeight - 0.75, 0.0f));
	cChest->SetShader("Shader3D");
	cChest->Init();
	cChest->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	cChest->SetScale(glm::vec3(0.5, 0.5, 0.5));
	// Add the chest to the cSolidObjectManager
	cSolidObjectManager->Add(cChest);

	// Initialise the chest detection zone
	fCheckHeight = cTerrain->GetHeight(0.0f, 0.0f);
	CChestZone* cChestZone = new CChestZone(glm::vec3(0.0f, fCheckHeight - 0.75, 0.0f));
	cChestZone->SetShader("Shader3D");
	cChestZone->Init();
	cChestZone->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	cChestZone->SetScale(glm::vec3(2, 0.5, 2));
	// Add the chest zone to the cSolidObjectManager
	cSolidObjectManager->Add(cChestZone);


	// Initialise door (portal) that goes to next level
	fCheckHeight = cTerrain->GetHeight(0.0f, -90.0f);
	CDoor* cDoor = new CDoor(glm::vec3(0.0f, fCheckHeight, -90.0f));
	cDoor->SetShader("Shader3D");
	cDoor->Init();
	cDoor->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	// Add the door (portal) to the cSolidObjectManager
	cDoor->SetScale(glm::vec3(0.03, 0.03, 0.03));
	cSolidObjectManager->Add(cDoor);


	// Initialise the first cEnemy3D
	fCheckHeight = cTerrain->GetHeight(-5.0f, -90.0f);
	CEnemylvl2* cEnemy3D = new CEnemylvl2(glm::vec3(-5.0f, fCheckHeight, -90.0f));
	cEnemy3D->SetShader("Shader3D");
	cEnemy3D->Init();
	cEnemy3D->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	//cEnemy3D->SetScale(glm::vec3(0.5f));

	// Assign a cPistol to the cEnemy3D
	CPistol* cEnemyPistol = new CPistol();
	// Set the position, rotation and scale of this weapon
	//cEnemyPistol->SetPosition(glm::vec3(0.05f, -0.075f, 0.5f));
	//cEnemyPistol->SetRotation(3.14159f, glm::vec3(0.0f, 1.0f, 0.0f));
	cEnemyPistol->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
	// Initialise the instance
	cEnemyPistol->Init();
	cEnemyPistol->SetShader("Shader3D_Model");
	cEnemy3D->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cEnemy3D);




	// Initialise the 2nd cEnemy3D
	fCheckHeight = cTerrain->GetHeight(5.0f, -90.0f);
	CEnemylvl2* cEnemy3D_2 = new CEnemylvl2(glm::vec3(5.0f, fCheckHeight, -90.0f));
	cEnemy3D_2->SetShader("Shader3D");
	cEnemy3D_2->Init();
	cEnemy3D_2->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	//cEnemy3D->SetScale(glm::vec3(0.5f));

	// Assign a cPistol to the cEnemy3D
	cEnemy3D_2->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cEnemy3D_2);



	// Initialise the 3rd cEnemy3D
	fCheckHeight = cTerrain->GetHeight(30.0f, 0.0f);
	CEnemylvl2* cEnemy3D_3 = new CEnemylvl2(glm::vec3(30.0f, fCheckHeight, 0.0f));
	cEnemy3D_3->SetShader("Shader3D");
	cEnemy3D_3->Init();
	cEnemy3D_3->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	//cEnemy3D->SetScale(glm::vec3(0.5f));

	// Assign a cPistol to the cEnemy3D
	cEnemy3D_3->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cEnemy3D_3);



	// Initialise the 4th cEnemy3D
	fCheckHeight = cTerrain->GetHeight(-30.0f, 0.0f);
	CEnemylvl2* cEnemy3D_4 = new CEnemylvl2(glm::vec3(-30.0f, fCheckHeight, 0.0f));
	cEnemy3D_4->SetShader("Shader3D");
	cEnemy3D_4->Init();
	cEnemy3D_4->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	//cEnemy3D->SetScale(glm::vec3(0.5f));

	// Assign a cPistol to the cEnemy3D
	cEnemy3D_4->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cEnemy3D_4);


	// Initialise the 5th cEnemy3D
	fCheckHeight = cTerrain->GetHeight(-30.0f, -30.0f);
	CEnemylvl2* cEnemy3D_5 = new CEnemylvl2(glm::vec3(-30.0f, fCheckHeight, -30.0f));
	cEnemy3D_5->SetShader("Shader3D");
	cEnemy3D_5->Init();
	cEnemy3D_5->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	//cEnemy3D->SetScale(glm::vec3(0.5f));

	// Assign a cPistol to the cEnemy3D
	cEnemy3D_5->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cEnemy3D_5);


	// Initialise the 6th cEnemy3D
	fCheckHeight = cTerrain->GetHeight(30.0f, -30.0f);
	CEnemylvl2* cEnemy3D_6 = new CEnemylvl2(glm::vec3(30.0f, fCheckHeight, -30.0f));
	cEnemy3D_6->SetShader("Shader3D");
	cEnemy3D_6->Init();
	cEnemy3D_6->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	//cEnemy3D->SetScale(glm::vec3(0.5f));

	// Assign a cPistol to the cEnemy3D
	cEnemy3D_6->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cEnemy3D_6);



	// Initialise the 7th cEnemy3D
	fCheckHeight = cTerrain->GetHeight(-50.0f, -30.0f);
	CEnemylvl2* cEnemy3D_7 = new CEnemylvl2(glm::vec3(-50.0f, fCheckHeight, -30.0f));
	cEnemy3D_7->SetShader("Shader3D");
	cEnemy3D_7->Init();
	cEnemy3D_7->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	//cEnemy3D->SetScale(glm::vec3(0.5f));

	// Assign a cPistol to the cEnemy3D
	cEnemy3D_7->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cEnemy3D_7);



	// Initialise the 8th cEnemy3D
	fCheckHeight = cTerrain->GetHeight(50.0f, -30.0f);
	CEnemylvl2* cEnemy3D_8 = new CEnemylvl2(glm::vec3(50.0f, fCheckHeight, -30.0f));
	cEnemy3D_8->SetShader("Shader3D");
	cEnemy3D_8->Init();
	cEnemy3D_8->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	//cEnemy3D->SetScale(glm::vec3(0.5f));

	// Assign a cPistol to the cEnemy3D
	cEnemy3D_8->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cEnemy3D_8);


	/*// Initialise CStructureBoundary3D entities 
	fCheckHeight = cTerrain->GetHeight(0.0f, 50.0f);
	CStructure3D* cStructure3D = new CStructure3D(glm::vec3(0.0f, fCheckHeight, 50.0f));
	cStructure3D->SetShader("Shader3D");
	cStructure3D->Init();
	cStructure3D->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	//cStructure3D->SetScale(glm::vec3(0.5f));

	// Add the cStructure3D to the cSolidObjectManager
	cSolidObjectManager->Add(cStructure3D);*/

	// Load the GUI Entities
	// Store the cGUI_Scene3D singleton instance here
	cGUI_Scene3D = CGUI_Scene3D::GetInstance();
	cGUI_Scene3D->Init();

	// Load the non-movable Entities with no collisions
	// Initialise the CEntityManager
	cEntityManager = CEntityManager::GetInstance(); //wwdawe
	cEntityManager->Init();

	return true;
}

/**
 @brief Update Update this instance
 @param dElapsedTime A const double variable contains the time since the last frame
 @return A bool variable
*/
bool CLevel1::Update(const double dElapsedTime)
{
	// Store the current position, if rollback is needed.
	cPlayer3D->StorePositionForRollback();

	if (background_music_lvl1)
	{
		// Background music
		cSoundController->PlaySoundByID(6);
		background_music_lvl1 = false;
	}

	if (cPlayer3D->sprint == true && cPlayer3D->stamina > 0)
	{
		((CCameraShake*)CCameraEffectsManager::GetInstance()->Get("CameraShake"))->bToBeUpdated = true;

		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_W))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::FORWARD, (float)dElapsedTime);
			sprintCheck = true;
		}
		else if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_S))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::BACKWARD, (float)dElapsedTime);
			sprintCheck = true;
		}
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_A))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::LEFT, (float)dElapsedTime);
			sprintCheck = true;
		}
		else if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_D))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::RIGHT, (float)dElapsedTime);
			sprintCheck = true;
		}
	}
	else
	{
		((CCameraShake*)CCameraEffectsManager::GetInstance()->Get("CameraShake"))->bToBeUpdated = false;

		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_W))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::FORWARD, (float)dElapsedTime);
		}
		else if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_S))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::BACKWARD, (float)dElapsedTime);
		}
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_A))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::LEFT, (float)dElapsedTime);
		}
		else if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_D))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::RIGHT, (float)dElapsedTime);
		}
	}

	if (sprintCheck == true)
	{
		cPlayer3D->stamina -= 20 * dElapsedTime;
	}

	if (cPlayer3D->sprint == false && cPlayer3D->stamina < 100)
	{
		cPlayer3D->stamina += 10 * dElapsedTime;
		sprintCheck = false;
	}

	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_SPACE))
		cPlayer3D->SetToJump();

	// Get keyboard updates for camera
	if (!cPlayer3D->IsCameraAttached())
	{
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_I))
			cCamera->ProcessKeyboard(CCamera::CAMERAMOVEMENT::FORWARD, (float)dElapsedTime);
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_K))
			cCamera->ProcessKeyboard(CCamera::CAMERAMOVEMENT::BACKWARD, (float)dElapsedTime);
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_J))
			cCamera->ProcessKeyboard(CCamera::CAMERAMOVEMENT::LEFT, (float)dElapsedTime);
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_L))
			cCamera->ProcessKeyboard(CCamera::CAMERAMOVEMENT::RIGHT, (float)dElapsedTime);

		// Get mouse updates
		cCamera->ProcessMouseMovement((float)cMouseController->GetMouseDeltaX(), (float)cMouseController->GetMouseDeltaY());
		cCamera->ProcessMouseScroll((float)cMouseController->GetMouseScrollStatus(CMouseController::SCROLL_TYPE::SCROLL_TYPE_YOFFSET));
	}
	else
	{
		// Get mouse updates
		cPlayer3D->ProcessRotate((float)cMouseController->GetMouseDeltaX(), (float)cMouseController->GetMouseDeltaY());
		cCamera->ProcessMouseScroll((float)cMouseController->GetMouseScrollStatus(CMouseController::SCROLL_TYPE::SCROLL_TYPE_YOFFSET));
	}

	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_0))
	{
		if (cPlayer3D->IsCameraAttached())
			cPlayer3D->AttachCamera();
		else
			cPlayer3D->AttachCamera(cCamera);

		// Reset the key so that it will not repeat until the key is released and pressed again
		CKeyboardController::GetInstance()->ResetKey(GLFW_KEY_0);
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_1))
	{
		cPlayer3D->SetCurrentWeapon(0);
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_2))
	{
		cPlayer3D->SetCurrentWeapon(1);
	}
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_8))
	{
		bool bStatus = CCameraEffectsManager::GetInstance()->Get("CameraShake")->GetStatus();
		CCameraEffectsManager::GetInstance()->Get("CameraShake")->SetStatus(!bStatus);

		// Reset the key so that it will not repeat until the key is released and pressed again
		CKeyboardController::GetInstance()->ResetKey(GLFW_KEY_8);
	}
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_9))
	{
		bool bStatus = CCameraEffectsManager::GetInstance()->Get("CrossHair")->GetStatus();
		CCameraEffectsManager::GetInstance()->Get("CrossHair")->SetStatus(!bStatus);

		// Reset the key so that it will not repeat until the key is released and pressed again
		CKeyboardController::GetInstance()->ResetKey(GLFW_KEY_9);
	}

	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
	{
		cPlayer3D->sprint = true;
	}
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_LEFT_SHIFT))
	{
		cPlayer3D->sprint = false;
	}

	// Get mouse button updates
	if (cMouseController->IsButtonReleased(CMouseController::BUTTON_TYPE::LMB))
	{
		cPlayer3D->DischargeWeapon();
	}
	if (cMouseController->IsButtonPressed(CMouseController::BUTTON_TYPE::RMB))
	{
		// Switch on Scope mode and zoom in
		cCamera->fZoom = 1.0f;
		CCameraEffectsManager::GetInstance()->Get("ScopeScreen")->SetStatus(true);
	}
	else if (cMouseController->IsButtonReleased(CMouseController::BUTTON_TYPE::RMB))
	{
		// Switch off Scope mode and zoom out
		cCamera->fZoom = 45.0f;
		CCameraEffectsManager::GetInstance()->Get("ScopeScreen")->SetStatus(false);
	}


	// Player dead
	if (cPlayer3D->playerlostallhealth == true)
	{
		CCameraEffectsManager::GetInstance()->Get("Youlose")->SetStatus(true);
		if (checkplayerdie == 0)
		{
			cSoundController->StopSound();
			background_music_lvl1 = false;
			checkplayerdie += 1;
		}
		else
		{
			cSoundController->PlaySoundByID(5);
		}
		cout << "you lose" << losegame << endl;
		cPlayer3D->playerhealthbelow30 = false;
		//cSolidObjectManager->youlose = true;
		cGUI_Scene3D->gameOver = true;
		losegame = true;
	}

	if (cPlayer3D->playerhealthbelow30 == true)
	{
		CCameraEffectsManager::GetInstance()->Get("Lowhealth")->SetStatus(true);
		cSoundController->PlaySoundByID(4);
	}

	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_ESCAPE))
	{
		cSoundController->StopSound();
	}

	else
	{
		CCameraEffectsManager::GetInstance()->Get("Lowhealth")->SetStatus(false);
	}

	// Update the Solid Objects
	cSolidObjectManager->Update(dElapsedTime);

	// Update the projectiles
	cProjectileManager->Update(dElapsedTime);

	// Check for collisions among Entities and also with Projectiles
	cSolidObjectManager->CheckForCollision();
	if (cSolidObjectManager->lvl1_portal_bool == true)
	{
		cSoundController->StopSound();
		teleport_2_lvl2 = true;
		CCameraEffectsManager::GetInstance()->Get("LoadingScreen")->SetStatus(true);
	}

	// Update the entities
	cEntityManager->Update(dElapsedTime);

	// Call the cGUI_Scene3D's update method
	cGUI_Scene3D->Update(dElapsedTime);

	// Post Update the mouse controller
	cMouseController->PostUpdate();

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CLevel1::PreRender(void)
{
	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(0.0f, 0.1f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 @brief Render Render this instance
 */
void CLevel1::Render(void)
{
	// Get the camera view and projection
	glm::mat4 view = CCamera::GetInstance()->GetViewMatrix();;
	glm::mat4 projection = glm::perspective(	glm::radians(CCamera::GetInstance()->fZoom),
												(float)cSettings->iWindowWidth / (float)cSettings->iWindowHeight,
												0.1f, 1000.0f);
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the SkyBox
	cSkyBox->SetView(view);
	cSkyBox->SetProjection(projection);
	cSkyBox->PreRender();
	cSkyBox->Render();
	cSkyBox->PostRender();

	// Render the Terrain
	cTerrain->SetView(view);
	cTerrain->SetProjection(projection);
	cTerrain->PreRender();
	cTerrain->Render();
	cTerrain->PostRender();

	// Render the entities
	cEntityManager->SetView(view);
	cEntityManager->SetProjection(projection);
	cEntityManager->Render();

	// Render the solid objects
	cSolidObjectManager->SetView(view);
	cSolidObjectManager->SetProjection(projection);
	cSolidObjectManager->Render();

	// Render the projectiles
	cProjectileManager->SetView(view);
	cProjectileManager->SetProjection(projection);
	cProjectileManager->PreRender();
	cProjectileManager->Render();
	cProjectileManager->PostRender();

	// now draw the mirror quad with screen texture
	// --------------------------------------------
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

	cGUI_Scene3D->SetProjection(projection);
	// Call the cGUI_Scene3D's PreRender()
	cGUI_Scene3D->PreRender();
	// Call the cGUI_Scene3D's Render()
	cGUI_Scene3D->Render();
	// Call the cGUI_Scene3D's PostRender()
	cGUI_Scene3D->PostRender();

	glEnable(GL_DEPTH_TEST);

	return;
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CLevel1::PostRender(void)
{
}