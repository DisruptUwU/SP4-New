#pragma once

/**
 CPlay3DGameState
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"

#include "..\Scene3D\Level1.h"

class CLevel1GameState : public CGameStateBase
{
public:
	// Constructor
	CLevel1GameState(void);
	// Destructor
	~CLevel1GameState(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

	double gameovertimer = 0.0f;

protected:
	// The handler to the CLevel4 instance
	CLevel1* CLevel1;
};
