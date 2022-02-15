#pragma once

/**
 CPlay3DGameState
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"

#include "..\Scene3D\Scene3D.h"
#include "..\Scene3D\Level3.h"

class CLevel3GameState : public CGameStateBase
{
public:
	// Constructor
	CLevel3GameState(void);
	// Destructor
	~CLevel3GameState(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

	double gameovertimer;

protected:
	// The handler to the CLevel4 instance
	CLevel3* CLevel3;
};
