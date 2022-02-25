/**
 CWeaponInfo
 By: Toh Da Jun
 Date: Sep 2020
 */
#pragma once

// Include CEntity3D
#include <Primitives/Entity3D.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CSolidObject
#include "../Entities/SolidObject.h"

// Include Projectile
#include "Projectile.h"

class CWeaponInfo : public CEntity3D
{
public:
	// Default Constructor
	CWeaponInfo(void);
	// Destructor
	virtual ~CWeaponInfo(void);

	// Set the time between shots
	virtual void SetTimeBetweenShots(const double dTimeBetweenShots);
	// Set the firing rate in rounds per min
	virtual void SetFiringRate(const int iFiringRate);
	// Set the firing flag
	virtual void SetCanFire(const bool bFire);

	// Get the time between shots
	virtual double GetTimeBetweenShots(void) const;
	// Get the firing rate
	virtual int GetFiringRate(void) const;
	// Get the firing flag
	virtual bool GetCanFire(void) const;

	// Initialise this instance to default values
	virtual bool Init(void);
	// Update the elapsed time
	virtual bool Update(const double dt);
	// Discharge this weapon
	virtual bool Discharge(glm::vec3 vec3Position, glm::vec3 vec3Front, CSolidObject* pSource = NULL);

	// Set model
	virtual void SetModel(glm::mat4 model);
	// Set view
	virtual void SetView(glm::mat4 view);
	// Set projection
	virtual void SetProjection(glm::mat4 projection);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

	// Print Self
	virtual void PrintSelf(void);

protected:
	// The time between shots in milliseconds
	double dTimeBetweenShots;
	// The elapsed time (between shots) in milliseconds
	double dElapsedTime;
	// Boolean flag to indicate if weapon can fire now
	bool bFire;

	// For Renderings
	unsigned int iIndicesSize;
};
