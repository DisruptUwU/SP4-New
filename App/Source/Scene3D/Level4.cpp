/**
 CScene3D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Level4.h"

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
CLevel4::CLevel4(void)
	: cSettings(NULL)
	, cKeyboardController(NULL)
	, cMouseController(NULL)
	, cFPSCounter(NULL)
	, cGUI_Scene3D(NULL)
	, cSoundController(NULL)
	, cSolidObjectManager(NULL)
	, cProjectileManager(NULL)
	, cEntityManager(NULL)
	, cPlayer3D(NULL)
	, cCamera(NULL)
	, cSkyBox(NULL)
	, cTerrain(NULL)
{
}

/**
 @brief Destructor
 */
CLevel4::~CLevel4(void)
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
bool CLevel4::Init(void)
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
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Dragontheme.ogg"), 2, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\dragonroar1.ogg"), 3, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\dragonroar2.ogg"), 4, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\dragonroar3.ogg"), 5, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\dragonroar4.ogg"), 6, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\dragonroar5.ogg"), 7, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\dragonroar6.ogg"), 8, true);


	// Load the Environment Entities
	// Load the SkyBox
	cSkyBox = CSkyBox::GetInstance();
	// Set a shader to this class instance of CSkyBox
	cSkyBox->SetShader("Shader3D_SkyBox");
	cSkyBox->Init();

	// Load the Ground
	cTerrain = CTerrain::GetInstance();
	cTerrain->SetShader("Shader3D_Terrain");
	cTerrain->InitTerrain(4);
	// Set the size of the Terrain
	cTerrain->SetRenderSize(50.0f, 5.0f, 50.0f);

	// Load the movable Entities
	// Initialise the CSolidObjectManager
	cSolidObjectManager = CSolidObjectManager::GetInstance();
	cSolidObjectManager->Init();

	// Initialise the cPlayer3D
	cPlayer3D = CPlayer3D::GetInstance();
	cPlayer3D->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	cPlayer3D->SetShader("Shader3D");
	cPlayer3D->Init();
	cPlayer3D->InitCollider("Shader3D_Line", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	cPlayer3D->AttachCamera(cCamera);

	// Add the cPlayer3D to the cSolidObjectManager
	cSolidObjectManager->Add(cPlayer3D);

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

	// Initialise the cEnemy3D
	CDragon* cDragon = new CDragon(glm::vec3(0.0f, 5.0f, -30.0f));
	cDragon->SetShader("Shader3D");
	cDragon->Init();
	cDragon->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec3(-1.5, 0, -2.5), glm::vec3(1.5, 5.5, 2.5));
	cDragon->SetScale(glm::vec3(0.1f));

	// Assign a cPistol to the cEnemy3D
	CPistol* cEnemyPistol = new CPistol();
	cEnemyPistol->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
	// Initialise the instance
	cEnemyPistol->Init();
	cEnemyPistol->SetShader("Shader3D_Model");
	cEnemyPistol->SetTimeBetweenShots(0.5);
	cDragon->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cDragon);

	// Load the GUI Entities
	// Store the cGUI_Scene3D singleton instance here
	cGUI_Scene3D = CGUI_Scene3D::GetInstance();
	cGUI_Scene3D->Init();

	cSolidObjectManager->cDragon = cDragon;
	cGUI_Scene3D->cDragon = cDragon;

	// Load the non-movable Entities with no collisions
	// Initialise the CEntityManager
	cEntityManager = CEntityManager::GetInstance();
	cEntityManager->Init();

	float fCheckHeight = cTerrain->GetHeight(0.0f, 0.0f);
	cDoor = new CDoor(glm::vec3(0.0f, fCheckHeight, 0.0f));
	cDoor->SetShader("Shader3D");
	cDoor->Init();
	cDoor->SetScale(glm::vec3(0.03, 0.03, 0.03));
	cDoor->SetStatus(false);
	cEntityManager->Add(cDoor);

	bNextLevel = false;

	return true;
}

/**
 @brief Update Update this instance
 @param dElapsedTime A const double variable contains the time since the last frame
 @return A bool variable
*/
bool CLevel4::Update(const double dElapsedTime)
{
	// Store the current position, if rollback is needed.
	cPlayer3D->StorePositionForRollback();

	if (checkfirstroar == 0)
	{
		cSoundController->PlaySoundByID(3);
		checkfirstroar += 1;
	}
	else
	{
		//
	}

	if (playSounds == true)
	{
		gametimer += dElapsedTime;
		cSoundController->PlaySoundByID(2);
	}

	else
	{
		gametimer = 0;
	}

	if (gametimer >= 10)
	{
		if (checkroar1 == 0)
		{
			cSoundController->PlaySoundByID(5);
			checkroar1 += 1;
			spawnpower1 = true;
			CCameraEffectsManager::GetInstance()->Get("Jumpscare1")->SetStatus(true);
		}
		else
		{
			//return;
			spawnpower1 = false;
		}
	}
	if (gametimer >= 20)
	{
		if (checkroar2 == 0)
		{
			cSoundController->PlaySoundByID(6);
			checkroar2 += 1;
			CCameraEffectsManager::GetInstance()->Get("Jumpscare2")->SetStatus(true);
		}
		else
		{

		}
	}
	if (gametimer >= 30)
	{
		if (checkroar3 == 0)
		{
			cSoundController->PlaySoundByID(4);
			checkroar3 += 1;
			spawnpower2 = true;
			CCameraEffectsManager::GetInstance()->Get("Jumpscare3")->SetStatus(true);
		}
		else
		{
			spawnpower2 = false;
		}
	}
	if (gametimer >= 40)
	{
		if (checkroar4 == 0)
		{
			cSoundController->PlaySoundByID(5);
			checkroar4 += 1;
			CCameraEffectsManager::GetInstance()->Get("Jumpscare4")->SetStatus(true);
		}
		else
		{
			//return;
		}
	}

	if (gametimer >= 60)
	{
		gametimer = 0;
		checkroar1 = 0;
		checkroar2 = 0;
		checkroar3 = 0;
		checkroar4 = 0;
		spawnpower1 = false;
		spawnpower2 = false;
	}

	if (spawnpower1 == true)
	{
		CHealthup* cHealthup = new CHealthup(glm::vec3(5, 1, -5));
		cHealthup->SetShader("Shader3D");
		cHealthup->Init();
		cHealthup->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		// Add the cStructure3D to the cSolidObjectManager
		cSolidObjectManager->Add(cHealthup);
	}

	if (spawnpower2 == true) // //
	{
		CIncreaseDmg* cIncreaseDmg = new CIncreaseDmg(glm::vec3(8, 1, -8));
		cIncreaseDmg->SetShader("Shader3D");
		cIncreaseDmg->Init();
		cIncreaseDmg->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		cSolidObjectManager->Add(cIncreaseDmg);
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

	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_F) && cPlayer3D->ult >= 100 && cPlayer3D->ultActive == false) {
		cPlayer3D->ultActive = true;
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
		cCamera->fZoom = 20.0f;
		CCameraEffectsManager::GetInstance()->Get("ScopeScreen")->SetStatus(true);
	}
	else if (cMouseController->IsButtonReleased(CMouseController::BUTTON_TYPE::RMB))
	{
		// Switch off Scope mode and zoom out
		cCamera->fZoom = 45.0f;
		CCameraEffectsManager::GetInstance()->Get("ScopeScreen")->SetStatus(false);
	}

	// Update the Solid Objects
	cSolidObjectManager->Update(dElapsedTime);

	// Update the projectiles
	cProjectileManager->Update(dElapsedTime);

	// Check for collisions among Entities and also with Projectiles
	cSolidObjectManager->CheckForCollision();

	// Update the entities
	cEntityManager->Update(dElapsedTime);

	// Call the cGUI_Scene3D's update method
	cGUI_Scene3D->Update(dElapsedTime);

	// Post Update the mouse controller
	cMouseController->PostUpdate();

	if (cSolidObjectManager->DragonKilled == true)
	{
		playSounds = false;
		if (checklastroar == 0)
		{
			cSoundController->StopSound();
			checklastroar += 1;
		}

		else if (checklastroar == 1)
		{
			cSoundController->PlaySoundByID(8);
			checklastroar += 1;
		}

		else if (checklastroar > 1)
		{
			//
		}
	}

	if (cSolidObjectManager->Count() == 1)
	{
		// portal
		cDoor->SetStatus(true);
		
		if (cPlayer3D->GetPosition().x >= -1 && cPlayer3D->GetPosition().x <= 1 && cPlayer3D->GetPosition().z >= -1 && cPlayer3D->GetPosition().z <= 1)
		{
			bNextLevel = true;
			CCameraEffectsManager::GetInstance()->Get("LoadingScreen")->SetStatus(true);
		}
	}

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CLevel4::PreRender(void)
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
void CLevel4::Render(void)
{
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

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
void CLevel4::PostRender(void)
{
}