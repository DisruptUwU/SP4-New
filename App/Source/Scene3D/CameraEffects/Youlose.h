/**
 CBloodScreen
 By: Toh Da Jun
 Date: Apr 2020
 */
#pragma once

// Include CCameraEffects
#include "CameraEffects.h"

class CYoulose : public CCameraEffects
{
public:
	// Constructor
	CYoulose(void);
	// Destructor
	virtual ~CYoulose(void);

	// Initialise this class instance
	bool Init(void);
};
