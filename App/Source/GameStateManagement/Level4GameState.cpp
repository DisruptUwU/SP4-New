// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

#include "Level4GameState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CLevel4GameState::CLevel4GameState(void)
	: CLevel4(NULL)
{

}

/**
 @brief Destructor
 */
CLevel4GameState::~CLevel4GameState(void)
{

}

/**
 @brief Init this class instance
 */
bool CLevel4GameState::Init(void)
{
	cout << "CPlay3DGameState::Init()\n" << endl;

	// Initialise the CScene3D instance
	CLevel4 = CLevel4::GetInstance();
	if (CLevel4->Init() == false)
	{
		cout << "Failed to load CLevel4" << endl;
		return false;
	}

	return true;
}

/**
 @brief Update this class instance
 */
bool CLevel4GameState::Update(const double dElapsedTime)
{
	if (CLevel4->bNextLevel == true)
	{

		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		cout << "Loading level 4" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("Level4GameState");
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
	CLevel4->Update(dElapsedTime);

	return true;
}

/**
 @brief Render this class instance
 */
void CLevel4GameState::Render(void)
{
	//cout << "CPlay3DGameState::Render()\n" << endl;

	// Call the CScene3D's Pre-Render method
	CLevel4->PreRender();

	// Call the CScene3D's Render method
	CLevel4->Render();

	// Call the CScene3D's PostRender method
	CLevel4->PostRender();
}

/**
 @brief Destroy this class instance
 */
void CLevel4GameState::Destroy(void)
{
	cout << "CPlay3DGameState::Destroy()\n" << endl;

	// Destroy the CScene3D instance
	if (CLevel4)
	{
		CLevel4->Destroy();
		CLevel4 = NULL;
	}
}