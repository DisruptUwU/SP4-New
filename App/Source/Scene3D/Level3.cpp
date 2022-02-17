/**
 CScene3D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Level3.h"

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
///testing
// Include CCameraShake
#include "CameraEffects/CameraShake.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CLevel3::CLevel3(void)
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
{
}

/**
 @brief Destructor
 */
CLevel3::~CLevel3(void)
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
bool CLevel3::Init(void)
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

	// Load the Environment Entities
	// Load the SkyBox
	cSkyBox = CSkyBox::GetInstance();
	// Set a shader to this class instance of CSkyBox
	cSkyBox->SetShader("Shader3D_SkyBox");
	cSkyBox->Init();

	// Load the Ground
	cTerrain = CTerrain::GetInstance();
	cTerrain->SetShader("Shader3D_Terrain");
	cTerrain->InitLevel3();
	// Set the size of the Terrain
	cTerrain->SetRenderSize(70.0f, 5.0f, 70.0f); //

	// Load the movable Entities
	// Initialise the CSolidObjectManager
	cSolidObjectManager = CSolidObjectManager::GetInstance();
	cSolidObjectManager->Init();

	// Initialise the cPlayer3D
	cPlayer3D = CPlayer3D::GetInstance();
	cPlayer3D->SetPosition(glm::vec3(-2.4f, 5.7f, -28.1f));
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
	float fCheckHeight = cTerrain->GetHeight(0.0f, -10.0f);
	cHydra = new CHydra(glm::vec3(0.0f, 0.5f, 0.0f));
	cHydra->SetShader("Shader3D");
	cHydra->Init(1);
	cHydra->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec3(-2, 1.5, -0.5), glm::vec3(0.25, 3, 0.5));
	//cHydra->bIsDisplayed = false;
	//cEnemy3D->SetScale(glm::vec3(0.5f));
	// Assign a cPistol to the cEnemy3D

	CPistol* cEnemyPistol = new CPistol();
	// Set the position, rotation and scale of this weapon
	//cEnemyPistol->SetPosition(glm::vec3(0.05f, -0.075f, 0.5f));
	//cEnemyPistol->SetPosition(glm::vec3(0.05f, 20.f, -0.3f));
	//cEnemyPistol->SetRotation(60, glm::vec3(0.0f, 1.0f, 0.0f));
	cEnemyPistol->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
	// Initialise the instance
	cEnemyPistol->Init();
	cEnemyPistol->SetShader("Shader3D_Model");
	cHydra->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cHydra);

	CFinalNPC* cFinalNPC = new CFinalNPC(glm::vec3(10.0f, fCheckHeight, -5));
	cFinalNPC->SetShader("Shader3D");
	cFinalNPC->Init();
	cFinalNPC->InitCollider("Shader3D_Line", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	//cNPC->SetScale(glm::vec3(0.5f));
	// Add the cGenerator to the cSolidObjectManager
	cSolidObjectManager->Add(cFinalNPC);

	//// Initialise a CStructure3D
	//fCheckHeight = cTerrain->GetHeight(2.0f, -2.0f);
	//cSpeed = new CSpeed(glm::vec3(2.0f, fCheckHeight, -2.0f));
	//cSpeed->SetShader("Shader3D");
	//cSpeed->Init();
	//cSpeed->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	//// Add the cStructure3D to the cSolidObjectManager
	//cSolidObjectManager->Add(cSpeed);

	// Initialise a CStructure3D
	fCheckHeight = cTerrain->GetHeight(2.0f, -2.0f);
	cJumpscaretrap = new CJumpscaretrap(glm::vec3(2.0f, fCheckHeight, -2.0f));
	cJumpscaretrap->SetShader("Shader3D");
	cJumpscaretrap->Init();
	cJumpscaretrap->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	//cStructure3D->SetScale(glm::vec3(0.5f));

	// Add the cStructure3D to the cSolidObjectManager
	cSolidObjectManager->Add(cJumpscaretrap);

	// Initialise a CStructure3D
	fCheckHeight = cTerrain->GetHeight(2.0f, -2.0f);
	CHealthup* cHealthup = new CHealthup(glm::vec3(5, fCheckHeight, -5));
	cHealthup->SetShader("Shader3D");
	cHealthup->Init();
	cHealthup->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	//cStructure3D->SetScale(glm::vec3(0.5f));

	// Add the cStructure3D to the cSolidObjectManager
	cSolidObjectManager->Add(cHealthup);

	//// Initialise a CStructure3D
	//CBloodbath* cBloodbath = new CBloodbath(glm::vec3(0, 8, 0));
	//cBloodbath->SetShader("Shader3D");
	//cBloodbath->Init();
	//cBloodbath->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)); // 0.0.1.0
	//cBloodbath->SetRotation(-90.f, glm::vec3(1.0f, 0.0f, 0.0f));
	//cBloodbath->SetScale(glm::vec3(15, 15, 15));

	//cSolidObjectManager->Add(cBloodbath);

	cSolidObjectManager->cHydra = cHydra;

	// Load the GUI Entities
	// Store the cGUI_Scene3D singleton instance here
	cGUI_Scene3D = CGUI_Scene3D::GetInstance();
	cGUI_Scene3D->Init();

	cGUI_Scene3D->cHydra = cHydra;

	cGUI_Scene3D->cSolidObjectManager = cSolidObjectManager;

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
bool CLevel3::Update(const double dElapsedTime)
{
	// Store the current position, if rollback is needed.
	cPlayer3D->StorePositionForRollback();
	if (cPlayer3D->sprint == true && cPlayer3D->stamina > 0) {
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_W))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::FORWARD, (float)dElapsedTime);
			((CCameraShake*)CCameraEffectsManager::GetInstance()->Get("CameraShake"))->bToBeUpdated = true;
			sprintCheck = true;
		}
		else if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_S))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::BACKWARD, (float)dElapsedTime);
			((CCameraShake*)CCameraEffectsManager::GetInstance()->Get("CameraShake"))->bToBeUpdated = true;
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
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_W))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::FORWARD, (float)dElapsedTime);
			((CCameraShake*)CCameraEffectsManager::GetInstance()->Get("CameraShake"))->bToBeUpdated = true;
		}
		else if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_S))
		{
			cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::BACKWARD, (float)dElapsedTime);
			((CCameraShake*)CCameraEffectsManager::GetInstance()->Get("CameraShake"))->bToBeUpdated = true;
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

	if (cPlayer3D->NearFinalNPC == true)
	{
		if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_E))
		{
			/*if (cPlayer3D->FinalNPCDialogueStage <= 6)
			{
				cPlayer3D->FinalNPCDialogueStage++;
			}
			if (cPlayer3D->FinalNPCDialogueStage >= 6)
			{
				cPlayer3D->FinalNPCDialogueStage = 6;
			}*/
		}
	}

	if (sprintCheck == true) {
		cPlayer3D->stamina -= 20 * dElapsedTime;
	}

	if (cPlayer3D->sprint == false && cPlayer3D->stamina < 100)
	{
		cPlayer3D->stamina += 10 * dElapsedTime;
		sprintCheck = false;
	}

	if (cSolidObjectManager->HydraKilled == true)
	{
		if (checkportal == 0)
		{
			spawnportal = true;
			checkportal += 1;
		}

		else
		{
			spawnportal = false;
		}
	}

	if (spawnportal == true)
	{
		float fCheckHeight = cTerrain->GetHeight(0.0f, -10.0f);
		fCheckHeight = cTerrain->GetHeight(2.0f, -2.0f);
		CDoor* cDoor = new CDoor(glm::vec3(-15.1, -0.5, 28.9)); //2
		cDoor->SetShader("Shader3D");
		cDoor->Init();
		cDoor->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
		//cSpeed->SetRotation(-90.f, glm::vec3(1.0f, 0.0f, 0.0f));
		cDoor->SetScale(glm::vec3(0.03, 0.03, 0.03));

		cSolidObjectManager->Add(cDoor);

		cout << "Door has spawned " << spawnportal << endl;
	}

	if (cSolidObjectManager->wenttodoor == true)//push
	{
		gotolevel4 = true;
	}

	if (cSolidObjectManager->cHydra->HydraBossHp <= 50 && cSolidObjectManager->cHydra->HydraBossHp >= 31)
	{
		//cSolidObjectManager->healthbelow50 = true;
		cHydra->moreaggresivepart1 = true;
	}

	if (cSolidObjectManager->cHydra->HydraBossHp <= 30 && cHydra->moreaggresivepart2 == false)
	{
		//cSolidObjectManager->healthbelow50 = true;
		cHydra->moreaggresivepart1 = false;
		cHydra->changingform = true;
		cHydra->Init(2); //new place
		cHydra->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
			glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
		//cHydra->SetScale(glm::vec3(1 * 3 * dElapsedTime, 1 * 3 * dElapsedTime, 1 * 3 * dElapsedTime));
	}

	if (cHydra->moreaggresivepart2 == true)
	{
		cSolidObjectManager->moreaggresivepart2 = true;

		if (checkaggresion == 0)
		{
			cHydra->Init(3); //
			cHydra->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
				glm::vec3(-2.5, -0.3, -0.5), glm::vec3(0, 1.5, 0.5));
			checkaggresion += 1;
		}

		else
		{
			//spawnportal = false;
		}

		//cHydra->moreaggresivepart2 = false;
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
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_R))
	{
		cPlayer3D->GetWeapon()->Reload();
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

	if (cPlayer3D->playerlostallhealth == true)
	{
		CCameraEffectsManager::GetInstance()->Get("Youlose")->SetStatus(true);
		cSoundController->PlaySoundByID(5);
		cout << "you lose" << losegame << endl;
		cPlayer3D->playerhealthbelow30 = false;
		cSolidObjectManager->youlose = true;
	}

	if (cPlayer3D->jumpscaretrapped == true)
	{
		CCameraEffectsManager::GetInstance()->Get("Youlose")->SetStatus(true);
		cSoundController->PlaySoundByID(5);
	}

	if (cSolidObjectManager->youlose == true)
	{
		//CCameraEffectsManager::GetInstance()->Get("Youlose")->SetStatus(true);
		//cout << "you lose" << losegame << endl;
		losegame = true;
	}

	//if (cGUI_Scene3D->gameOver == true)
	//{
	//	losegame = true;
	//}

	if (cPlayer3D->playerhealthbelow30 == true)
	{
		CCameraEffectsManager::GetInstance()->Get("Lowhealth")->SetStatus(true);
		//cSoundController->PlaySoundByID(4);
	}

	else
	{
		CCameraEffectsManager::GetInstance()->Get("Lowhealth")->SetStatus(false);
	}

	//if (cPlayer3D->jumpscaretrapped == true)
	//{
	//	CCameraEffectsManager::GetInstance()->Get("Youlose")->SetStatus(true);
	//	//cSoundController->PlaySoundByID(4);
	//}

	//else
	//{
	//	CCameraEffectsManager::GetInstance()->Get("Youlose")->SetStatus(false);
	//}



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

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CLevel3::PreRender(void)
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
void CLevel3::Render(void)
{
	// Part 1: Render for the minimap by binding to framebuffer and render to color texture
	//         But the camera is move to top-view of the scene

	// Backup some key settings for the camera and player
	glm::vec3 storePlayerPosition = cPlayer3D->GetPosition();
	float storeCameraYaw = cCamera->fYaw;
	float storeCameraPitch = cCamera->fPitch;
	glm::vec3 storeCameraPosition = cCamera->vec3Position;
	// Adjust camera yaw and pitch so that it is looking from a top-view of the terrain
	cCamera->fYaw += 180.0f;
	cCamera->fPitch = -90.0f;
	// We store the player's position into the camera as we want the minimap to focus on the player
	cCamera->vec3Position = glm::vec3(storePlayerPosition.x, 10.0f, storePlayerPosition.z);
	// Recalculate all the camera vectors. 
	// We disable pitch constrains for this specific case as we want the camera to look straight down
	cCamera->ProcessMouseMovement(0, 0, false);
	// Generate the view and projection
	glm::mat4 playerView = cCamera->GetViewMatrix();
	glm::mat4 playerProjection = glm::perspective(glm::radians(45.0f),
		(float)cSettings->iWindowWidth / (float)cSettings->iWindowHeight,
		0.1f, 1000.0f);

	// Set the camera parameters back to the previous values
	cCamera->fYaw = storeCameraYaw;
	cCamera->fPitch = storeCameraPitch;
	cCamera->vec3Position = storeCameraPosition;
	cCamera->ProcessMouseMovement(0, 0, true); // call this to make sure it updates its camera vectors, note that we disable pitch constrains for this specific case (otherwise we can't reverse camera's pitch values)

	// Activate the minimap system
	CMinimap::GetInstance()->Activate();
	// Setup the rendering environment
	CMinimap::GetInstance()->PreRender();

	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

	// Render the Terrain
	cTerrain->SetView(playerView);
	cTerrain->SetProjection(playerProjection);
	cTerrain->PreRender();
	cTerrain->Render();
	cTerrain->PostRender();

	// Render the entities
	cEntityManager->SetView(playerView);
	cEntityManager->SetProjection(playerProjection);
	cEntityManager->Render();

	// Render the entities for the minimap
	cSolidObjectManager->SetView(playerView);
	cSolidObjectManager->SetProjection(playerProjection);
	cSolidObjectManager->Render();

	// Deactivate the cMinimap so that we can render as per normal
	CMinimap::GetInstance()->Deactivate();

	// Part 2: Render the entire scene as per normal
	// Get the camera view and projection
	glm::mat4 view = CCamera::GetInstance()->GetViewMatrix();;
	glm::mat4 projection = glm::perspective(glm::radians(CCamera::GetInstance()->fZoom),
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

	return;
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CLevel3::PostRender(void)
{
}