/**
 CBloodScreen
 By: Toh Da Jun
 Date: Apr 2020
 */
#pragma once

// Include CCameraEffects
#include "CameraEffects.h"

class CYouwin : public CCameraEffects
{
public:
	// Constructor
	CYouwin(void);
	// Destructor
	virtual ~CYouwin(void);

	// Initialise this class instance
	bool Init(void);
};
