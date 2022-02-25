// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

#include "Level1GameState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CLevel1GameState::CLevel1GameState(void)
	: CLevel1(NULL)
{

}

/**
 @brief Destructor
 */
CLevel1GameState::~CLevel1GameState(void)
{

}

/**
 @brief Init this class instance
 */
bool CLevel1GameState::Init(void)
{
	cout << "CPlay3DGameState::Init()\n" << endl;

	// Initialise the CScene3D instance
	CLevel1 = CLevel1::GetInstance();
	if (CLevel1->Init() == false)
	{
		cout << "Failed to load CLevel1" << endl;
		return false;
	}

	return true;
}

/**
 @brief Update this class instance
 */
bool CLevel1GameState::Update(const double dElapsedTime)
{
	if (CLevel1->losegame == true)
	{
		gameovertimer += dElapsedTime;
	}

	if (gameovertimer >= 2.5)
	{
		CLevel1->losegame = false;
		gameovertimer = 0;

		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading MenuState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		return true;
	}
	if (CLevel1->teleport_2_lvl2 == true)
	{
		CLevel1->teleport_2_lvl2 == false;
		// Reset the CKeyboardController
		// CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading Level 2" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("Level2GameState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		return true;
	}
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading MenuState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		return true;
	}
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_F10))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading PauseState" << endl;
		CGameStateManager::GetInstance()->SetPauseGameState("PauseState");
	}

	// Call the CScene3D's Update method
	CLevel1->Update(dElapsedTime);

	return true;
}

/**
 @brief Render this class instance
 */
void CLevel1GameState::Render(void)
{
	//cout << "CPlay3DGameState::Render()\n" << endl;

	// Call the CScene3D's Pre-Render method
	CLevel1->PreRender();

	// Call the CScene3D's Render method
	CLevel1->Render();

	// Call the CScene3D's PostRender method
	CLevel1->PostRender();
}

/**
 @brief Destroy this class instance
 */
void CLevel1GameState::Destroy(void)
{
	cout << "CPlay3DGameState::Destroy()\n" << endl;

	// Destroy the CScene3D instance
	if (CLevel1)
	{
		CLevel1->Destroy();
		CLevel1 = NULL;
	}
}