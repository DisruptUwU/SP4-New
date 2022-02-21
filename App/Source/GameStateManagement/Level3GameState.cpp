// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

#include "Level3GameState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CLevel3GameState::CLevel3GameState(void)
	: CLevel3(NULL)
{

}

/**
 @brief Destructor
 */
CLevel3GameState::~CLevel3GameState(void)
{

}

/**
 @brief Init this class instance
 */
bool CLevel3GameState::Init(void)
{
	cout << "CPlay3DGameState::Init()\n" << endl;

	// Initialise the CScene3D instance
	CLevel3 = CLevel3::GetInstance();
	if (CLevel3->Init() == false)
	{
		cout << "Failed to load CLevel3" << endl;
		return false;
	}

	gameovertimer = 0;

	return true;
}

/**
 @brief Update this class instance
 */
bool CLevel3GameState::Update(const double dElapsedTime)
{
	if (CLevel3->losegame == true)
	{
		gameovertimer += dElapsedTime;
	}

	if (gameovertimer >= 2.5)
	{
		CLevel3->losegame = false;
		cout << "Loading MenuState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		return true;
	}

	if (CLevel3->gotolevel4 == true)
	{
		CLevel3->gotolevel4 = false;
		cout << "Loading level 4" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("FinalGameState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		return true;
	}

	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
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
	CLevel3->Update(dElapsedTime);

	return true;
}

/**
 @brief Render this class instance
 */
void CLevel3GameState::Render(void)
{
	//cout << "CPlay3DGameState::Render()\n" << endl;

	// Call the CScene3D's Pre-Render method
	CLevel3->PreRender();

	// Call the CScene3D's Render method
	CLevel3->Render();

	// Call the CScene3D's PostRender method
	CLevel3->PostRender();
}

/**
 @brief Destroy this class instance
 */
void CLevel3GameState::Destroy(void)
{
	cout << "CPlay3DGameState::Destroy()\n" << endl;

	// Destroy the CScene3D instance
	if (CLevel3)
	{
		CLevel3->Destroy();
		CLevel3 = NULL;
	}
}