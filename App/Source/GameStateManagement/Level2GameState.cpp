// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

#include "Level2GameState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CLevel2GameState::CLevel2GameState(void)
	: CLevel2(NULL)
{

}

/**
 @brief Destructor
 */
CLevel2GameState::~CLevel2GameState(void)
{

}

/**
 @brief Init this class instance
 */
bool CLevel2GameState::Init(void)
{
	cout << "CPlay3DGameState::Init()\n" << endl;

	// Initialise the CScene3D instance
	CLevel2 = CLevel2::GetInstance();
	if (CLevel2->Init() == false)
	{
		cout << "Failed to load CLevel4" << endl;
		return false;
	}

	return true;
}

/**
 @brief Update this class instance
 */
bool CLevel2GameState::Update(const double dElapsedTime)
{
	if (CLevel2->gotolevel3 == true)
	{
		CLevel2->gotolevel3 = false;
		cout << "Loading level 3" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("Level3GameState");
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
	CLevel2->Update(dElapsedTime);

	return true;
}

/**
 @brief Render this class instance
 */
void CLevel2GameState::Render(void)
{
	//cout << "CPlay3DGameState::Render()\n" << endl;

	// Call the CScene3D's Pre-Render method
	CLevel2->PreRender();

	// Call the CScene3D's Render method
	CLevel2->Render();

	// Call the CScene3D's PostRender method
	CLevel2->PostRender();
}

/**
 @brief Destroy this class instance
 */
void CLevel2GameState::Destroy(void)
{
	cout << "CPlay3DGameState::Destroy()\n" << endl;

	// Destroy the CScene3D instance
	if (CLevel2)
	{
		CLevel2->Destroy();
		CLevel2 = NULL;
	}
}