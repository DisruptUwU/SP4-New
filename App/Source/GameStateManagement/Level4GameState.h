#pragma once

/**
 CPlay3DGameState
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"

#include "..\Scene3D\Scene3D.h"
#include "..\Scene3D\Level4.h"

class CLevel4GameState : public CGameStateBase
{
public:
	// Constructor
	CLevel4GameState(void);
	// Destructor
	~CLevel4GameState(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

protected:
	// The handler to the CLevel4 instance
	CLevel4* CLevel4;
};
