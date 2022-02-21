// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

#include "FinalGameState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CFinalState::CFinalState(void)
	: CLevelFinal(NULL)
{

}

/**
 @brief Destructor
 */
CFinalState::~CFinalState(void)
{

}

/**
 @brief Init this class instance
 */
bool CFinalState::Init(void)
{
	cout << "CPlay3DGameState::Init()\n" << endl;

	// Initialise the CScene3D instance
	CLevelFinal = CLevelFinal::GetInstance();
	if (CLevelFinal->Init() == false)
	{
		cout << "Failed to load CLevel4" << endl;
		return false;
	}

	return true;
}

/**
 @brief Update this class instance
 */
bool CFinalState::Update(const double dElapsedTime)
{
	/*if (CLevelFinal->gotolevel3 == true)
	{
		CLevelFinal->gotolevel3 = false;
		cout << "Loading level 3" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("Level3GameState");
		CGameStateManager::GetInstance()->OffPauseGameState();
		return true;
	}*/

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
	CLevelFinal->Update(dElapsedTime);

	return true;
}

/**
 @brief Render this class instance
 */
void CFinalState::Render(void)
{
	//cout << "CPlay3DGameState::Render()\n" << endl;

	// Call the CScene3D's Pre-Render method
	CLevelFinal->PreRender();

	// Call the CScene3D's Render method
	CLevelFinal->Render();

	// Call the CScene3D's PostRender method
	CLevelFinal->PostRender();
}

/**
 @brief Destroy this class instance
 */
void CFinalState::Destroy(void)
{
	cout << "CPlay3DGameState::Destroy()\n" << endl;

	// Destroy the CScene3D instance
	if (CLevelFinal)
	{
		CLevelFinal->Destroy();
		CLevelFinal = NULL;
	}
}