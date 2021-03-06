/**
 CPlayer3D
 By: Toh Da Jun
 Date: Apr 2020
 */
#include "Player3D.h"

 // Include ShaderManager
#include "RenderControl/ShaderManager.h"

// Include ImageLoader
#include "System/ImageLoader.h"

//For allowing creating of Mesh 
#include "Primitives/MeshBuilder.h"

#include <iostream>
using namespace std;

/**
 @brief Default Constructor
 */
CPlayer3D::CPlayer3D(void)
	: vec3Up(glm::vec3(0.0f, 1.0f, 0.0f))
	, vec3Right(glm::vec3(1.0f, 1.0f, 0.0f))
	, vec3WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
	, fYaw(-90.0f)
	, fPitch(0.0f)
	, fMouseSensitivity(0.1f)
	, cCamera(NULL)
	, cTerrain(NULL)
	, cPrimaryWeapon(NULL)
	, cSecondaryWeapon(NULL)
	, iCurrentWeapon(0)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
{
	// Set the default position so it is above the ground
	vec3Position = glm::vec3(0.0f, 0.5f, 0.0f);

	// Update the player vectors
	UpdatePlayerVectors();
}

/**
 @brief Constructor with vectors
 @param position A const glm::vec3 variable which contains the position of the camera
 @param up A const glm::vec3 variable which contains the up direction of the camera
 @param yaw A const float variable which contains the yaw of the camera
 @param pitch A const float variable which contains the pitch of the camera
 */
CPlayer3D::CPlayer3D(	const glm::vec3 vec3Position,
						const glm::vec3 vec3Front,
						const float fYaw,
						const float fPitch)
	: vec3Up(glm::vec3(0.0f, 1.0f, 0.0f))
	, vec3Right(glm::vec3(1.0f, 1.0f, 0.0f))
	, vec3WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
	, fYaw(fYaw)
	, fPitch(fPitch)
	, fMouseSensitivity(0.1f)
	, cCamera(NULL)
	, cTerrain(NULL)
	, cPrimaryWeapon(NULL)
	, cSecondaryWeapon(NULL)
	, iCurrentWeapon(0)
{
	mesh = NULL;

	this->vec3Position = vec3Position;
	this->vec3Front = vec3Front;
	this->fYaw = fYaw;
	this->fPitch = fPitch;

	// Update the player vectors
	UpdatePlayerVectors();
}

/**
 @brief Destructor
 */
CPlayer3D::~CPlayer3D(void)
{
	if (cSecondaryWeapon)
	{
		delete cSecondaryWeapon;
		cSecondaryWeapon = NULL;
	}

	if (cPrimaryWeapon)
	{
		delete cPrimaryWeapon;
		cPrimaryWeapon = NULL;
	}

	if (cTerrain)
	{
		// We set it to NULL only since it was declared somewhere else
		cTerrain = NULL;
	}
	
	if (cCamera)
	{
		// We set it to NULL only since it was declared somewhere else
		cCamera = NULL;
	}

	if (cInventoryManager)
	{
		cInventoryManager = NULL;
	}
}

/**
 @brief Initialise this class instance
 @return true is successfully initialised this class instance, else false
 */
bool CPlayer3D::Init(void)
{
	// Call the parent's Init()
	CSolidObject::Init();

	// Set the type
	SetType(CEntity3D::TYPE::PLAYER);

	// Generate and bind the VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	mesh = CMeshBuilder::GenerateBox(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		1.0f, 1.0f, 1.0f);

	// load and create a texture 
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Scene3D_Player.tga", false);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene3D_Player.tga" << endl;
		return false;
	}

	// Store the handler to the CTerrain
	cTerrain = CTerrain::GetInstance();
	// Update the y-axis position of the player
	vec3Position.y = cTerrain->GetHeight(vec3Position.x, vec3Position.z);

	// Get the handler to the CInventoryManager instance
	cInventoryManager = CInventoryManager::GetInstance();

	// Set the Physics to fall status by default
	cPhysics3D.SetStatus(CPhysics3D::STATUS::FALL);

	return true;
}

/**
 @brief Set model
 @param model A const glm::mat4 variable containing the model for this class instance
 */
void CPlayer3D::SetModel(const glm::mat4 model)
{
	this->model = model;
}

/**
 @brief Set view
 @param view A const glm::mat4 variable containing the model for this class instance
 */
void CPlayer3D::SetView(const glm::mat4 view)
{
	this->view = view;
}

/**
 @brief Set projection
 @param projection A const glm::mat4 variable containing the model for this class instance
 */
void CPlayer3D::SetProjection(const glm::mat4 projection)
{
	this->projection = projection;
}

/**
@brief Returns the view matrix calculated using Euler Angles and the LookAt Matrix
@return A glm::mat4 variable which contains the view matrix
*/
glm::mat4 CPlayer3D::GetViewMatrix(void) const
{
	return glm::lookAt(vec3Position, vec3Position + vec3Front, vec3Up);
}

/**
 @brief Attach a camera to this class instance
 @param cCamera A CCamera* variable which contains the camera
 */
void CPlayer3D::AttachCamera(CCamera* cCamera)
{
	// Set the camera to the player
	this->cCamera = cCamera;

	// Update the camera's attributes with the player's attributes
	if (cCamera)
	{
		cCamera->vec3Position = vec3Position;
		cCamera->vec3Front = vec3Front;
		cCamera->vec3Up = vec3Up;
		cCamera->vec3Right = vec3Right;
		cCamera->fYaw = fYaw;
		cCamera->fPitch = fPitch;
	}
}

/**
 @brief Check if a camera ia attached to this class instance
 @return true if a camera is attached, else false
 */
bool CPlayer3D::IsCameraAttached(void)
{
	if (cCamera)
		return true;
	return false;
}

/**
 @brief Set Weapon to this class instance
 @param iSlot A const int variable which contains the weapon info to check for. 0 == Primary, 1 == Secondary
 @param cWeaponInfo A CWeaponInfo* variable which contains the weapon info
 */
void CPlayer3D::SetWeapon(const int iSlot, CWeaponInfo* cWeaponInfo)
{
	if (iSlot == 0)
		cPrimaryWeapon = cWeaponInfo;
	else if (iSlot == 1)
		cSecondaryWeapon = cWeaponInfo;
}

/**
 @brief Get Weapon of this class instance
 @param iSlot A const int variable which contains the weapon info to check for. 0 == Primary, 1 == Secondary
 */
CWeaponInfo* CPlayer3D::GetWeapon(void) const
{
	if (iCurrentWeapon == 0)
		return cPrimaryWeapon;
	else if (iCurrentWeapon == 1)
		return cSecondaryWeapon;

	return NULL;
}

/**
 @brief Set current weapon
 @param iSlot A const int variable which contains the weapon info to check for. 0 == Primary, 1 == Secondary
 */
void CPlayer3D::SetCurrentWeapon(const int iSlot)
{
	iCurrentWeapon = iSlot;
}

/**
 @brief Discharge weapon
 @param iSlot A const int variable which contains the weapon info to check for. 0 == Primary, 1 == Secondary
 @return A bool variable
 */
bool CPlayer3D::DischargeWeapon(void) const
{
	if ((iCurrentWeapon == 0) && (cPrimaryWeapon))
	{
		return cPrimaryWeapon->Discharge(vec3Position, vec3Front, (CSolidObject*)this);
	}
	else if ((iCurrentWeapon == 1) && (cSecondaryWeapon))
	{
		return cSecondaryWeapon->Discharge(vec3Position, vec3Front, (CSolidObject*)this);
	}
	//return NULL;
}

/**
 @brief Set to Jump
 */
void CPlayer3D::SetToJump(void)
{
	if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::IDLE)
	{
		cPhysics3D.SetStatus(CPhysics3D::STATUS::JUMP);
		cPhysics3D.SetInitialVelocity(glm::vec3(0.0f, 1.2f, 0.0f));
	}
}

/**
 @brief Processes input received from any keyboard-like input system as player movements. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
 @param direction A const Player_Movement variable which contains the movement direction of the camera
 @param deltaTime A const float variable which contains the delta time for the realtime loop
 */
void CPlayer3D::ProcessMovement(const PLAYERMOVEMENT direction, const float deltaTime)
{
	if (cantMove == true)
		return;

	float velocity = fMovementSpeed * deltaTime;

	if (trapped == true)
	{
		if (trapTimer > 0)
		{
			trapTimer -= 1 * deltaTime;
			velocity = fMovementSpeed * 0 * deltaTime;
		}
		else if (trapTimer <= 0)
		{
			trapTimer = 3;
			trapped = false;
		}
	}
	else if (trapped == false)
	{
		if (stamina <= 0)
		{
			if (speedPower == true)
			{
				velocity = fMovementSpeed * 5 * deltaTime * ultSpeed;
			}
			else
			{
				velocity = fMovementSpeed * 2 * deltaTime * ultSpeed;
			}
		}
		else
		{
			if (sprint == false)
			{
				if (speedPower == true)
				{
					velocity = fMovementSpeed * 5 * deltaTime * ultSpeed;
				}
				else
				{
					velocity = fMovementSpeed * 2 * deltaTime * ultSpeed;
				}
			}
			else if (sprint == true)
			{
				if (speedPower == true)
				{
					velocity = fMovementSpeed * 10 * deltaTime * ultSpeed;
				}
				else
				{
					velocity = fMovementSpeed * 4 * deltaTime * ultSpeed;
				}
			}
		}
	}

	if (direction == PLAYERMOVEMENT::FORWARD)
		vec3Position += vec3Front * velocity;
	if (direction == PLAYERMOVEMENT::BACKWARD)
		vec3Position -= vec3Front * velocity;
	if (direction == PLAYERMOVEMENT::LEFT)
		vec3Position -= vec3Right * velocity;
	if (direction == PLAYERMOVEMENT::RIGHT)
		vec3Position += vec3Right * velocity;
}

/**
 @brief Processes input received from a mouse input system as player rotation. Expects the offset value in both the x and y direction.
 @param xoffset A const float variable which contains the x axis of the mouse movement
 @param yoffset A const float variable which contains the y axis of the mouse movement
 @param constrainPitch A const bool variable which indicates if the pitch will be constrained at upright positions
 */
void CPlayer3D::ProcessRotate(float fXOffset, float fYOffset, const bool constrainPitch)
{
	fXOffset *= fMouseSensitivity;
	fYOffset *= fMouseSensitivity;

	fYaw += fXOffset;
	fPitch += fYOffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (fPitch > 89.0f)
			fPitch = 89.0f;
		if (fPitch < -89.0f)
			fPitch = -89.0f;
	}

	// Update vec3Front, Right and vec3Up Vectors using the updated Euler angles
	UpdatePlayerVectors();
}

/**
 @brief Update the elapsed time
 @param dt A const double variable containing the elapsed time since the last frame
 @return A bool variable
 */
bool CPlayer3D::Update(const double dElapsedTime)
{
	if (cPrimaryWeapon)
		cPrimaryWeapon->Update(dElapsedTime);
	if (cSecondaryWeapon)
		cSecondaryWeapon->Update(dElapsedTime);

	// Update the Jump/Fall
	UpdateJumpFall(dElapsedTime);

	// If the camera is attached to this player, then update the camera
	if (cCamera)
	{
		cCamera->vec3Position = vec3Position;
		cCamera->vec3Front = vec3Front;
		cCamera->vec3Up = vec3Up;
		cCamera->vec3Right = vec3Right;
		cCamera->fYaw = fYaw;
		cCamera->fPitch = fPitch;
	}

	// Constraint the player's position
	Constraint();

	if (jumpscaretraptimer <= 0)
	{
		jumpscaretraptimer = 1.5;
		jumpscaretrapped = false;
	}

	if (jumpscaretrapped == true)
	{
		jumpscaretraptimer -= dElapsedTime;
	}

	if (DmgUpTimer <= 0)
	{
		AtkIncrease = false;
		DmgUpTimer = 30;
	}

	if (AtkIncrease == true) {
		DmgUpTimer -= 1 * dElapsedTime;
		Damage = 20; 
		DamagetoHydra = 10;
	}
	else
	{
		Damage = 10; //10
		DamagetoHydra = 5; //5
	}
	
	if (DefUpTimer <= 0)
	{
		DefenceIncrease = false;
		DefUpTimer = 30;
	}

	if (ultTimer <= 0 && ult <= 0)
	{
		ultTimer = 3;
		ultActive = false;
		ultSpeed = 1;
		ultDamage = 0;
		cout << "power down" << endl;
	}

	if (ultActive == true)
	{
		ultTimer -= 1 * dElapsedTime;
		ultSpeed = 5;
		ultDamage = 30;
		ult -= 35 * dElapsedTime;
	}
	else
	{
		if (ult < 100)
		{
			ult += 10 * dElapsedTime;
		}

	}

	if (Hit == true)
	{
		ImmunityTimer -= 1 * dElapsedTime;
		cInventoryItem->Remove(0);
	}

	if (ImmunityTimer <= 0)
	{
		ImmunityTimer = 1;
		Hit = false;
	}

	if (DefenceIncrease == true) {
		DefUpTimer -= 1 * dElapsedTime;
		if (healthdownbyhydra == true)
		{
			if (Hit == false)
			{
				cInventoryItem = cInventoryManager->GetItem("Health");
				cInventoryItem->Remove(5); //float
				Hit = true;
				healthdownbyhydra = false;
			}
			else
			{
				cInventoryItem->Remove(0);
			}
		}
		if (healthdownbyhydramore == true)
		{
			if (Hit == false)
			{
				cInventoryItem = cInventoryManager->GetItem("Health");
				cInventoryItem->Remove(15); //float
				Hit = true;
				healthdownbyhydramore = false;
			}
			else
			{
				cInventoryItem->Remove(0);
			}
		}
	}
	else
	{
		if (healthdownbyhydra == true)
		{
			if (Hit == false)
			{
				cInventoryItem = cInventoryManager->GetItem("Health");
				cInventoryItem->Remove(10); //float //10 //40
				Hit = true;
				healthdownbyhydra = false;
			}
			else
			{
				cInventoryItem->Remove(0);
			}
		}
		if (healthdownbyhydramore == true)
		{
			if (Hit == false)
			{
				cInventoryItem = cInventoryManager->GetItem("Health");
				cInventoryItem->Remove(30); //float
				Hit = true;
				healthdownbyhydramore = false;
			}
			else
			{
				cInventoryItem->Remove(0);
			}
		}
	}

	if (collectCoin == true)
	{
		cInventoryItem = cInventoryManager->GetItem("Coins");
		cInventoryItem->Add(1);
		collectCoin = false;
	}

	cInventoryItem = cInventoryManager->GetItem("Health");
	if (cInventoryItem->GetCount() <= 30) //getmaxcount
	{
		playerhealthbelow30 = true;
	}
	else
	{
		playerhealthbelow30 = false;
	}

	if (cInventoryItem->GetCount() <= 0) //getmaxcount
	{
		playerlostallhealth = true;
	}

	//if (cInventoryItem->GetCount() >= 100) //getmaxcount
	//{
	//	cInventoryItem->GetCount() = 100;
	//}

	if (healthPower == true)
	{
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Add(40); //float
		healthPower = false;
	}

	if (speedPower == false) // later move this in update
	{
		speedTimer = 0;
	}

	if (speedPower == true)
	{
		speedTimer += dElapsedTime;
		/*cout << "Speed power activated = " << speedPower << endl;
		cout << "Speed time left = " << speedTimer << endl;*/
	}

	if (speedTimer >= 3.5)
	{
		//speedTimer = 0;
		speedPower = false;
		/*cout << "Speed time left = " << speedTimer << endl;*/
	}

	CSolidObject::Update(dElapsedTime);

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CPlayer3D::PreRender(void)
{
	CSolidObject::PreRender();
}

/**
 @brief Render Render this instance
 */
void CPlayer3D::Render(void)
{
	// Don't render the player if the camera is attached to it
	if (cCamera)
		return;

	CSolidObject::Render();
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CPlayer3D::PostRender(void)
{
	CSolidObject::PostRender();
}

/**
 @brief Calculates the front vector from the Camera's (updated) Euler Angles
 */
void CPlayer3D::UpdatePlayerVectors(void)
{
	// Calculate the new vec3Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(fYaw)) * cos(glm::radians(fPitch));
	front.y = sin(glm::radians(fPitch));
	front.z = sin(glm::radians(fYaw)) * cos(glm::radians(fPitch));
	vec3Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	// Normalize the vectors, because their length gets closer to 0 the more 
	// you look up or down which results in slower movement.
	vec3Right = glm::normalize(glm::cross(vec3Front, vec3WorldUp));  
	vec3Up = glm::normalize(glm::cross(vec3Right, vec3Front));

	// If the camera is attached to this player, then update the camera
	if (cCamera)
	{
		cCamera->vec3Front = vec3Front;
		cCamera->vec3Up = vec3Up;
		cCamera->vec3Right = vec3Right;
		cCamera->fYaw = fYaw;
		cCamera->fPitch = fPitch;
	}
}

/**
 @brief Constraint the player's position
 */
void CPlayer3D::Constraint(void)
{
	// Prevent player from leaving map
	if (vec3Position.x < cTerrain->GetMinPos().x + 1)
	{
		vec3Position.x = cTerrain->GetMinPos().x + 1;
	}
	else if (vec3Position.x > cTerrain->GetMaxPos().x - 1)
	{
		vec3Position.x = cTerrain->GetMaxPos().x - 1;
	}
	if (vec3Position.z < cTerrain->GetMinPos().z + 1)
	{
		vec3Position.z = cTerrain->GetMinPos().z + 1;
	}
	else if (vec3Position.z > cTerrain->GetMaxPos().z -1)
	{
		vec3Position.z = cTerrain->GetMaxPos().z - 1;
	}

	// If the player is not jumping nor falling, then we snap his position to the terrain
	if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::IDLE)
	{
		vec3Position.y = cTerrain->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;
	}
	else
	{
		// If the player is jumping nor falling, then we only snap his position 
		// if the player's y-coordinate is below the cTerrain 
		float fCheckHeight = cTerrain->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;

		// If the player is below the terrain, then snap to the terrain height
		if (fCheckHeight > vec3Position.y)
			vec3Position.y = fCheckHeight;
	}

}

/**
 @brief Update Jump or Fall
 @param deltaTime A const float variable which contains the delta time for the realtime loop
 */
void CPlayer3D::UpdateJumpFall(const double dElapsedTime)
{
	if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::JUMP)
	{
		// Update the elapsed time to the physics engine
		cPhysics3D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics3D.Update(dElapsedTime);
		// Get the displacement from the physics engine and update the player position
		vec3Position = vec3Position + cPhysics3D.GetDisplacement();

		// If the player is still jumping and the initial velocity has reached zero or below zero, 
		// then it has reach the peak of its jump
		if ((cPhysics3D.GetStatus() == CPhysics3D::STATUS::JUMP) && (cPhysics3D.GetDeltaDisplacement().y <= 0.0f))
		{
			// Set status to fall
			cPhysics3D.SetStatus(CPhysics3D::STATUS::FALL);
		}
	}
	else if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::FALL)
	{
		// Update the elapsed time to the physics engine
		cPhysics3D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics3D.Update(dElapsedTime);
		// Get the displacement from the physics engine and update the player position
		vec3Position = vec3Position + cPhysics3D.GetDisplacement();

		// Constaint the player's position to the terrain
		float fCheckHeight = cTerrain->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;

		// Set the Physics to idle status
		if (fCheckHeight > vec3Position.y)
		{
			vec3Position.y = fCheckHeight;
			cPhysics3D.SetStatus(CPhysics3D::STATUS::IDLE);
		}
	}
}

/**
 @brief Print Self
 */
void CPlayer3D::PrintSelf(void)
{
	cout << "CPlayer3D::PrintSelf()" << endl;
	cout << "========================" << endl;
}
