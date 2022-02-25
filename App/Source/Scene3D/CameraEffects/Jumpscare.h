/**
 CBloodScreen
 By: Toh Da Jun
 Date: Apr 2020
 */
#pragma once

// Include CCameraEffects
#include "CameraEffects.h"

class CJumpscare : public CCameraEffects
{
public:
	// Constructor
	CJumpscare(void);
	// Destructor
	virtual ~CJumpscare(void);

	// Initialise this class instance
	bool Init(int type);
};
