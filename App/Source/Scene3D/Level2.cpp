/**
 CScene3D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Level2.h"

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
CLevel2::CLevel2(void)
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
CLevel2::~CLevel2(void)
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
bool CLevel2::Init(void)
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
	cSoundController->LoadSound(FileSystem::getPath("Screech\\devilscream.ogg"), 4, true);
	cSoundController->LoadSound(FileSystem::getPath("Levelthemes\\Level2.ogg"), 5, true);
	cSoundController->LoadSound(FileSystem::getPath("ScarySoundeffects\\scarysoundeffect1.ogg"), 6, true);
	cSoundController->LoadSound(FileSystem::getPath("ScarySoundeffects\\scarysoundeffect2.ogg"), 7, true);
	cSoundController->LoadSound(FileSystem::getPath("ScarySoundeffects\\scarysoundeffect3.ogg"), 8, true);
	cSoundController->LoadSound(FileSystem::getPath("ScarySoundeffects\\scarysoundeffect4.ogg"), 9, true);
	cSoundController->LoadSound(FileSystem::getPath("ScarySoundeffects\\scarysoundeffect5.ogg"), 10, true);
	cSoundController->LoadSound(FileSystem::getPath("ScarySoundeffects\\scarysoundeffect6.ogg"), 11, true);

	// Load the Environment Entities
	// Load the SkyBox
	cSkyBox = CSkyBox::GetInstance();
	// Set a shader to this class instance of CSkyBox
	cSkyBox->SetShader("Shader3D_SkyBox");
	cSkyBox->Init();

	// Load the Ground
	cTerrain = CTerrain::GetInstance();
	cTerrain->SetShader("Shader3D_Terrain");
	cTerrain->InitLevel2();
	// Set the size of the Terrain
	cTerrain->SetRenderSize(200.0f, 5.0f, 200.0f);

	// Load the movable Entities
	// Initialise the CSolidObjectManager
	cSolidObjectManager = CSolidObjectManager::GetInstance();
	cSolidObjectManager->Init();

	// Initialise the cPlayer3D
	cPlayer3D = CPlayer3D::GetInstance();
	cPlayer3D->SetPosition(glm::vec3(-85.0f, 5.5f, -57.0f));
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
	CEnemylvl2* cEnemy3D = new CEnemylvl2(glm::vec3(0.0f, fCheckHeight, -10.0f));
	cEnemy3D->SetShader("Shader3D");
	cEnemy3D->Init();
	cEnemy3D->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//cEnemy3D->SetScale(glm::vec3(0.5f));

	// Initialise the cEnemy3D
	float fCheckHeight2 = cTerrain->GetHeight(0.0f, -10.0f);
	CEnemylvl2* cEnemy3D2 = new CEnemylvl2(glm::vec3(5.0f, fCheckHeight2, -10.0f));
	cEnemy3D2->SetShader("Shader3D");
	cEnemy3D2->Init();
	cEnemy3D2->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	float fCheckHeight3 = cTerrain->GetHeight(0.0f, -10.0f);
	CEnemylvl2* cEnemy3D3 = new CEnemylvl2(glm::vec3(10.0f, fCheckHeight3, -20.0f));
	cEnemy3D3->SetShader("Shader3D");
	cEnemy3D3->Init();
	cEnemy3D3->InitCollider("Shader3D_Line", glm::vec4(10.0f, 0.0f, 0.0f, 1.0f));

	float fCheckHeight4 = cTerrain->GetHeight(0.0f, -10.0f);
	CEnemylvl2* cEnemy3D4 = new CEnemylvl2(glm::vec3(5.0f, fCheckHeight4, -50.0f));
	cEnemy3D4->SetShader("Shader3D");
	cEnemy3D4->Init();
	cEnemy3D4->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	
		float fCheckHeight5 = cTerrain->GetHeight(0.0f, -10.0f);
		CEnemylvl2* cEnemy3D5 = new CEnemylvl2(glm::vec3(-40.0f, fCheckHeight4, -60.0f));
		cEnemy3D5->SetShader("Shader3D");
		cEnemy3D5->Init();
		cEnemy3D5->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		float fCheckHeight6 = cTerrain->GetHeight(0.0f, -10.0f);
		CEnemylvl2* cEnemy3D6 = new CEnemylvl2(glm::vec3(-60.0f, fCheckHeight4, -50.0f));
		cEnemy3D6->SetShader("Shader3D");
		cEnemy3D6->Init();
		cEnemy3D6->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		float fCheckHeight7 = cTerrain->GetHeight(0.0f, -10.0f);
		CEnemylvl2* cEnemy3D7 = new CEnemylvl2(glm::vec3(-17.0f, fCheckHeight4, -50.0f));
		cEnemy3D7->SetShader("Shader3D");
		cEnemy3D7->Init();
		cEnemy3D7->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	
		if (wavedead)
		{
			float fCheckHeight5 = cTerrain->GetHeight(0.0f, -10.0f);
			CDemon* cDemon = new CDemon(glm::vec3(30.0f, fCheckHeight5, -30.0f));
			cDemon->SetShader("Shader3D");
			//cDemon->SetRotation(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			cDemon->Init();
			cDemon->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			CPistol* cDemonPistol = new CPistol();
			cDemonPistol->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
			cDemonPistol->Init();
			cDemonPistol->SetShader("Shader3D_Model");
			cDemon->SetWeapon(0, cDemonPistol);

			cSolidObjectManager->cDemon = cDemon;

			cSolidObjectManager->Add(cDemon);
		}


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

	// Assign a cPistol to the cEnemy3D
	CPistol* cEnemyPistol2 = new CPistol();
	// Set the position, rotation and scale of this weapon
	//cEnemyPistol->SetPosition(glm::vec3(0.05f, -0.075f, 0.5f));
	//cEnemyPistol->SetRotation(3.14159f, glm::vec3(0.0f, 1.0f, 0.0f));
	cEnemyPistol2->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
	// Initialise the instance
	cEnemyPistol2->Init();
	cEnemyPistol2->SetShader("Shader3D_Model");
	cEnemy3D2->SetWeapon(0, cEnemyPistol2);


	CPistol* cEnemyPistol3 = new CPistol();
	cEnemyPistol3->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
	cEnemyPistol3->Init();
	cEnemyPistol3->SetShader("Shader3D_Model");
	cEnemy3D3->SetWeapon(0, cEnemyPistol3);

	CPistol* cEnemyPistol4 = new CPistol();
	cEnemyPistol4->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
	cEnemyPistol4->Init();
	cEnemyPistol4->SetShader("Shader3D_Model");
	cEnemy3D4->SetWeapon(0, cEnemyPistol4);

	CPistol* cEnemyPistol5 = new CPistol();
	cEnemyPistol5->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
	cEnemyPistol5->Init();
	cEnemyPistol5->SetShader("Shader3D_Model");
	cEnemy3D5->SetWeapon(0, cEnemyPistol5);

	CPistol* cEnemyPistol6 = new CPistol();
	cEnemyPistol6->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
	cEnemyPistol6->Init();
	cEnemyPistol6->SetShader("Shader3D_Model");
	cEnemy3D6->SetWeapon(0, cEnemyPistol6);

	CPistol* cEnemyPistol7 = new CPistol();
	cEnemyPistol7->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
	cEnemyPistol7->Init();
	cEnemyPistol7->SetShader("Shader3D_Model");
	cEnemy3D7->SetWeapon(0, cEnemyPistol7);



	 //Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cEnemy3D);
	cSolidObjectManager->Add(cEnemy3D2);
	cSolidObjectManager->Add(cEnemy3D3);
	cSolidObjectManager->Add(cEnemy3D4);
	cSolidObjectManager->Add(cEnemy3D5);
	cSolidObjectManager->Add(cEnemy3D6);
	cSolidObjectManager->Add(cEnemy3D7);


	//// Initialise a CStructure3D
	////fCheckHeight = cTerrain->GetHeight(2.0f, -2.0f);
	////CStructure3D* cStructure3D = new CStructure3D(glm::vec3(2.0f, fCheckHeight, -2.0f));
	////cStructure3D->SetShader("Shader3D");
	////cStructure3D->Init();
	////cStructure3D->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	////cStructure3D->SetScale(glm::vec3(0.5f));

	//// Add the cStructure3D to the cSolidObjectManager
	//cSolidObjectManager->Add(cStructure3D);

	// Initialise a coin
	fCheckHeight = cTerrain->GetHeight(2.0f, -2.0f);
	CCoin* ccoin1 = new CCoin(glm::vec3(2.0f, fCheckHeight, -2.0f));
	ccoin1->SetShader("Shader3D");
	ccoin1->Init();
	ccoin1->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	ccoin1->SetScale(glm::vec3(0.5f));

	// Add the cStructure3D to the cSolidObjectManager
	cSolidObjectManager->Add(ccoin1);

	// Load the GUI Entities
	// Store the cGUI_Scene3D singleton instance here
	cGUI_Scene3D = CGUI_Scene3D::GetInstance();
	cGUI_Scene3D->Init();

	// Load the non-movable Entities with no collisions
	// Initialise the CEntityManager
	cEntityManager = CEntityManager::GetInstance(); //wwdawe
	cEntityManager->Init();

	// Initialise the CRock3D
	//CRock3D* cRock3D = new CRock3D();
	//cRock3D->SetInstancingMode(false);
	//if (cRock3D->IsInstancedRendering() == true)
	//{
	//	cRock3D->SetScale(glm::vec3(5.0f));
	//	cRock3D->SetNumOfInstance(1000);
	//	cRock3D->SetSpreadDistance(100.0f);

	//	cRock3D->SetShader("Shader3D_Instancing");	// FOR INSTANCED RENDERING
	//}
	//else
	//{
	//	fCheckHeight = cTerrain->GetHeight(2.0f, 2.0f);
	//	cRock3D->SetPosition(glm::vec3(2.0f, fCheckHeight, 2.0f));
	//	cRock3D->SetScale(glm::vec3(0.5f));
	//	cRock3D->SetShader("Shader3DNoColour");			// FOR NORMAL RENDERING
	//}
	//if (cRock3D->Init() == true)
	//{
	//	cEntityManager->Add(cRock3D);
	//}
	//else
	//{
	//	delete cRock3D;
	//}

	// Initialise the CTreeKabak3D
	//CTreeKabak3D* cTreeKabak3D = new CTreeKabak3D(glm::vec3(0.0f, 0.0f, 0.0f));
	//cTreeKabak3D->SetInstancingMode(true);
	//if (cTreeKabak3D->IsInstancedRendering() == true)
	//{
	//	cTreeKabak3D->SetScale(glm::vec3(1.0f));
	//	cTreeKabak3D->SetNumOfInstance(100);
	//	cTreeKabak3D->SetSpreadDistance(100.0f);

	//	cTreeKabak3D->SetShader("Shader3D_Instancing");	// FOR INSTANCED RENDERING
	//}
	//if (cTreeKabak3D->Init() == true)
	//{
	//	cEntityManager->Add(cTreeKabak3D);
	//}
	//else
	//{
	//	delete cTreeKabak3D;
	//}

	// Initialise a CSpinTower
	//CSpinTower::Create();

	// Initialise a CHut_Concrete
	//fCheckHeight = cTerrain->GetHeight(-2.0f, 2.0f);
	//CHut_Concrete* cHut_Concrete = new CHut_Concrete(glm::vec3(-2.0f, fCheckHeight, 2.0f));
	//cHut_Concrete->SetShader("Shader3DNoColour");
	//cHut_Concrete->SetLODStatus(true);
	//cHut_Concrete->Init();
	//cHut_Concrete->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	//// Add the cHut_Concrete to the cSolidObjectManager
	//cSolidObjectManager->Add(cHut_Concrete);

	return true;
}

/**
 @brief Update Update this instance
 @param dElapsedTime A const double variable contains the time since the last frame
 @return A bool variable
*/
bool CLevel2::Update(const double dElapsedTime)
{
	// Store the current position, if rollback is needed.
	cPlayer3D->StorePositionForRollback();

	//if gametimer >= 0
	if (playSounds == true)
	{
		gametimer += dElapsedTime;
	}

	else
	{
		gametimer = 0;
	}

	if (gametimer >= 10) //10
	{
		if (checkSound1 == 0)
		{
			cSoundController->PlaySoundByID(6);
			checkSound1 += 1;
		}
		else
		{
			//return;
		}
	}
	if (gametimer >= 20)
	{
		if (checkSound2 == 0)
		{
			cSoundController->PlaySoundByID(8);
			checkSound2 += 1;
		}
		else
		{
			//return;
		}
	}
	if (gametimer >= 30)
	{
		if (checkSound3 == 0)
		{
			cSoundController->PlaySoundByID(9);
			checkSound3 += 1;
		}
		else
		{
			//return;
		}
	}
	if (gametimer >= 40)
	{
		if (checkSound4 == 0)
		{
			cSoundController->PlaySoundByID(11);
			checkSound4 += 1;
		}
		else
		{
			//return;
		}
	}

	if (gametimer >= 60)
	{
		gametimer = 0;
		checkSound1 = 0;
		checkSound2 = 0;
		checkSound3 = 0;
		checkSound4 = 0;
	}
	
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

	if (sprintCheck == true) {
		cPlayer3D->stamina -= 20 * dElapsedTime;
	}

	if (cPlayer3D->sprint == false && cPlayer3D->stamina < 100)
	{
		cPlayer3D->stamina += 10 * dElapsedTime;
		sprintCheck = false;
	}

	//if (cPlayer3D->ult < 100)
	//{
	//	cPlayer3D->ult += 10 * dElapsedTime;
	//}

	//if (cPlayer3D->ult >= 100 && cPlayer3D->ultTimer <= 0)
	//{
	//	cPlayer3D->ult = 0;
	//}

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
		cCamera->fZoom = 1.0f;
		CCameraEffectsManager::GetInstance()->Get("ScopeScreen")->SetStatus(true);
	}
	else if (cMouseController->IsButtonReleased(CMouseController::BUTTON_TYPE::RMB))
	{
		// Switch off Scope mode and zoom out
		cCamera->fZoom = 45.0f;
		CCameraEffectsManager::GetInstance()->Get("ScopeScreen")->SetStatus(false);
	}


	//if (CEnemy3D::GetStatus == false)
	//{
	//	wavedead = true;
	//}

	if (cSolidObjectManager->DeadEnemies >= 7) //change back to 7 later
	{
		if (checkDemonspawn == 0)
		{
			////wavedead = true;
			//float fCheckHeight5 = cTerrain->GetHeight(0.0f, -10.0f);
			//CDemon* cDemon = new CDemon(glm::vec3(30.0f, fCheckHeight5, -30.0f));
			//cDemon->SetShader("Shader3D");
			//cDemon->Init();
			//cDemon->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			//CPistol* cDemonPistol = new CPistol();
			//cDemonPistol->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
			//cDemonPistol->Init();
			//cDemonPistol->SetShader("Shader3D_Model");
			//cDemon->SetWeapon(0, cDemonPistol);

			//cSolidObjectManager->cDemon = cDemon;

			//cSolidObjectManager->Add(cDemon);
			//cout << "Die" << endl;
			//checkDemonspawn += 1;
			cSoundController->PlaySoundByID(4);
			cSoundController->PlaySoundByID(5);
			Demonspawn = true;
			checkDemonspawn += 1;
		}
		else
		{
			Demonspawn = false;
		}
	}

	if (Demonspawn == true)
	{
		//wavedead = true;
		float fCheckHeight5 = cTerrain->GetHeight(0.0f, -10.0f);
		CDemon* cDemon = new CDemon(glm::vec3(20.0f, fCheckHeight5, -30.0f));
		cDemon->SetShader("Shader3D");
		cDemon->Init();
		cDemon->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		CPistol* cDemonPistol = new CPistol();
		cDemonPistol->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
		cDemonPistol->Init();
		cDemonPistol->SetShader("Shader3D_Model");
		cDemon->SetWeapon(0, cDemonPistol);

		cSolidObjectManager->cDemon = cDemon;

		cSolidObjectManager->Add(cDemon);

		cGUI_Scene3D->cDemon = cDemon;
		cout << "Die" << endl;
	}

	if (cSolidObjectManager->DemonKilled == true)
	{
		//cSoundController->StopSound();
		if (portalcheck == 0)
		{
			cSoundController->StopSound();
			portalspawn = true;
			portalcheck += 1;
		}

		else if (portalcheck == 1)
		{
			cSoundController->PlaySoundByID(4);
			portalspawn = false;
			playSounds = false;
			portalcheck += 1;
		}

		else if (portalcheck >= 2)
		{
			portalspawn = false;
		}
	}

	if (portalspawn == true)
	{
		float fCheckHeight = cTerrain->GetHeight(0.0f, -10.0f);
		fCheckHeight = cTerrain->GetHeight(2.0f, -2.0f);
		CDoorlvl2* cDoor = new CDoorlvl2(glm::vec3(10, fCheckHeight, -30)); //2
		cDoor->SetShader("Shader3D");
		cDoor->Init();
		cDoor->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
		cDoor->SetRotation(-90.f, glm::vec3(1.0f, 0.0f, 0.0f));
		cDoor->SetScale(glm::vec3(0.03, 0.03, 0.03));

		cSolidObjectManager->Add(cDoor);
		cout << "door2" << endl;

	}

	if (cSolidObjectManager->Doorlevel2 == true && portalcheck >= 1)//extra check
	{
 		gotolevel3 = true;
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

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CLevel2::PreRender(void)
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
void CLevel2::Render(void)
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
	glm::mat4 playerProjection = glm::perspective(	glm::radians(45.0f),
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

	return;
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CLevel2::PostRender(void)
{
}