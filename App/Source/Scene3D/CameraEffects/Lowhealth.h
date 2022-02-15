/**
 CBloodScreen
 By: Toh Da Jun
 Date: Apr 2020
 */
#pragma once

// Include CCameraEffects
#include "CameraEffects.h"

class CLowhealth : public CCameraEffects
{
public:
	// Constructor
	CLowhealth(void);
	// Destructor
	virtual ~CLowhealth(void);

	// Initialise this class instance
	bool Init(void);
};
