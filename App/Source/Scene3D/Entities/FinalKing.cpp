/**
 CEnemy3D
 By: Toh Da Jun
 Date: Apr 2020
 */
#include "FinalKing.h"

 // Include ShaderManager
#include "RenderControl/ShaderManager.h"

// Include MeshBuilder
#include "Primitives/MeshBuilder.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include <iostream>
using namespace std;

// Set this to true if printing the debug info
#define _DEBUG_FSM false

/**
 @brief Default Constructor
 */
CFinalKing::CFinalKing(void)
	: vec3Up(glm::vec3(0.0f, 1.0f, 0.0f))
	, vec3Right(glm::vec3(1.0f, 1.0f, 0.0f))
	, vec3WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
	, fYaw(-90.0f)
	, fPitch(0.0f)
	, fRotationSensitivity(0.1f)
	, cCamera(NULL)
	, cPrimaryWeapon(NULL)
	, cSecondaryWeapon(NULL)
	, iCurrentWeapon(0)
	, cPlayer3D(NULL)
	, cTerrain(NULL)
{
	// Set the default position to the origin
	vec3Position = glm::vec3(0.0f, +fHeightOffset, 0.0f);

	// Update the vectors for this enemy
	UpdateEnemyVectors();
}

/**
 @brief Constructor with vectors
 @param vec3Position A const glm::vec3 variable which contains the position of the camera
 @param vec3Front A const glm::vec3 variable which contains the up direction of the camera
 @param yaw A const float variable which contains the yaw of the camera
 @param pitch A const float variable which contains the pitch of the camera
 */
CFinalKing::CFinalKing(const glm::vec3 vec3Position,
	const glm::vec3 vec3Front,
	const float fYaw,
	const float fPitch)
	: vec3Up(glm::vec3(0.0f, 1.0f, 0.0f))
	, vec3Right(glm::vec3(1.0f, 1.0f, 0.0f))
	, vec3WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
	, fYaw(fYaw)
	, fPitch(fPitch)
	, fRotationSensitivity(0.1f)
	, cCamera(NULL)
	, cPrimaryWeapon(NULL)
	, cSecondaryWeapon(NULL)
	, iCurrentWeapon(0)
	, cPlayer3D(NULL)
	, cTerrain(NULL)
{
	this->vec3Position = vec3Position;
	this->vec3Position.y += +fHeightOffset;
	this->vec3Front = vec3Front;
	this->fYaw = fYaw;
	this->fPitch = fPitch;

	// Update the vectors for this enemy
	UpdateEnemyVectors();
}

/**
 @brief Destructor
 */
CFinalKing::~CFinalKing(void)
{
	if (cTerrain)
	{
		// We set it to NULL only since it was declared somewhere else
		cTerrain = NULL;
	}

	if (cPlayer3D)
	{
		// We set it to NULL only since it was declared somewhere else
		cPlayer3D = NULL;
	}

	if (cCamera)
	{
		// We set it to NULL only since it was declared somewhere else
		cCamera = NULL;
	}

	// Delete the rendering objects in the graphics card
	glDeleteVertexArrays(1, &VAO);
}

/**
 @brief Initialise this class instance
 @return true is successfully initialised this class instance, else false
 */
bool CFinalKing::Init(void)
{
	// Call the parent's Init()
	CSolidObject::Init();

	// Set the type
	SetType(CEntity3D::TYPE::HEAVENLYKING);

	// Initialise the cPlayer3D
	cPlayer3D = CPlayer3D::GetInstance();

	// Generate and bind the VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	if (LoadModelAndTexture("Models/Final/HeavnlyKing.obj",
		"Models/Pistol/honeycombs_col.png",
		VAO, iTextureID, iIndicesSize) == false)
	{
		cout << "Unable to load model and texture" << endl;
	}

	// Store the handler to the terrain
	cTerrain = CTerrain::GetInstance();

	// Movement Control
	fMovementSpeed = 1.5f;
	iCurrentNumMovement = 0;
	iMaxNumMovement = 100;

	// Detection distance for player
	fDetectionDistance = 3.f;

	return true;
}

bool CFinalKing::LoadModelAndTexture(const char* filenameModel,
	const char* filenameTexture,
	GLuint& VAO,
	GLuint& iTextureID,
	GLuint& iIndicesSize)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<ModelVertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	std::string file_path = filenameModel;
	bool success = CLoadOBJ::LoadOBJ(file_path.c_str(), vertices, uvs, normals, true);
	if (!success)
	{
		cout << "Unable to load " << filenameModel << endl;
		return false;
	}

	CLoadOBJ::IndexVBO(vertices, uvs, normals, index_buffer_data, vertex_buffer_data);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(ModelVertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	iIndicesSize = index_buffer_data.size();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec3)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// load and create a texture 
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID(filenameTexture, false);
	if (iTextureID == 0)
	{
		cout << "Unable to load " << filenameTexture << endl;
		return false;
	}

	return true;
}


/**
 @brief Set model
 @param model A const glm::mat4 variable containing the model for this class instance
 */
void CFinalKing::SetModel(const glm::mat4 model)
{
	this->model = model;
}

/**
 @brief Set view
 @param view A const glm::mat4 variable containing the model for this class instance
 */
void CFinalKing::SetView(const glm::mat4 view)
{
	this->view = view;
}

/**
 @brief Set projection
 @param projection A const glm::mat4 variable containing the model for this class instance
 */
void CFinalKing::SetProjection(const glm::mat4 projection)
{
	this->projection = projection;
}

/**
 @brief Attach a camera to this class instance
 @param cCamera A CCamera* variable which contains the camera
 */
void CFinalKing::AttachCamera(CCamera* cCamera)
{
	// Set the camera to the player
	this->cCamera = cCamera;

	// Update the camera's attributes with the player's attributes
	if (cCamera)
	{
		cCamera->vec3Position = vec3Position;
		cCamera->vec3Front = vec3Front;
	}
}

/**
 @brief Check if a camera ia attached to this class instance
 @return true if a camera is attached, else false
 */
bool CFinalKing::IsCameraAttached(void)
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
void CFinalKing::SetWeapon(const int iSlot, CWeaponInfo* cWeaponInfo)
{
	if (iSlot == 0)
		cPrimaryWeapon = cWeaponInfo;
	else if (iSlot == 1)
		cSecondaryWeapon = cWeaponInfo;
}

/**
 @brief Get Weapon of this class instance
 @return The CWeaponInfo* value
 */
CWeaponInfo* CFinalKing::GetWeapon(void) const
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
void CFinalKing::SetCurrentWeapon(const int iSlot)
{
	iCurrentWeapon = iSlot;
}

/**
 @brief Discharge weapon
 @return A bool value
 */
bool CFinalKing::DischargeWeapon(void) const
{
	//if ((iCurrentWeapon == 0) && (cPrimaryWeapon))
	//{
	//	return cPrimaryWeapon->Discharge(0, vec3Position, vec3Front, (CSolidObject*)this);
	//}
	//else if ((iCurrentWeapon == 1) && (cSecondaryWeapon))
	//{
	//	return cSecondaryWeapon->Discharge(0, vec3Position, vec3Front, (CSolidObject*)this);
	//}
	return false;
}

/**
 @brief Processes input received from any keyboard-like input system as player movements. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
 @param direction A const Player_Movement variable which contains the movement direction of the camera
 @param deltaTime A const float variable which contains the delta time for the realtime loop
 */
void CFinalKing::ProcessMovement(const ENEMYMOVEMENT direction, const float deltaTime)
{
	float velocity = fMovementSpeed * deltaTime;
	if (direction == ENEMYMOVEMENT::FORWARD)
		vec3Position += vec3Front * velocity;
	if (direction == ENEMYMOVEMENT::BACKWARD)
		vec3Position -= vec3Front * velocity;
	if (direction == ENEMYMOVEMENT::LEFT)
		vec3Position -= vec3Right * velocity;
	if (direction == ENEMYMOVEMENT::RIGHT)
		vec3Position += vec3Right * velocity;

	// If the camera is attached to this player, then update the camera
	if (cCamera)
	{
		cCamera->vec3Position = vec3Position;
	}

	// Constraint the player's position
	Constraint();
}

/**
 @brief Processes input received from a mouse input system as player rotation. Expects the offset value in both the x and y direction.
 @param xoffset A const float variable which contains the x axis of the mouse movement
 */
void CFinalKing::ProcessRotate(const float fXOffset)
{
	// Update the yaw
	fYaw += fXOffset;// *fRotationSensitivity;

	// Update vec3Front, Right and vec3Up Vectors using the updated Euler angles
	UpdateEnemyVectors();
}

/**
 @brief Update the elapsed time
 @param dt A const double variable containing the elapsed time since the last frame
 @return A bool variable
 */
bool CFinalKing::Update(const double dElapsedTime)
{
	// Don't update if this entity is not active
	if (bStatus == false)
	{
		return false;
	}

	// Store the enemy's current position, if rollback is needed.
	StorePositionForRollback();

	switch (sCurrentFSM)
	{
	case FSM::IDLE:
		//cout << glm::distance(vec3Position, cPlayer3D->GetPosition()) << " vs " << fDetectionDistance << endl;
		if (glm::distance(vec3Position, cPlayer3D->GetPosition()) < fDetectionDistance)
		{
			cPlayer3D->NeartheKing = true;
		}
		else 
		{
			cPlayer3D->NeartheKing = false;
		}
		iFSMCounter++;
		break;
	default:
		break;
	}

	// Update the model
	model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	model = glm::translate(model, vec3Position);
	model = glm::scale(model, vec3Scale);
	model = glm::rotate(model, glm::radians(fYaw), glm::vec3(0.0f, 1.0f, 0.0f));

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CFinalKing::PreRender(void)
{
	// If this entity is not active, then skip this
	if (bStatus == false)
	{
		return;
	}

	CSolidObject::PreRender();
}

/**
 @brief Render Render this instance
 */
void CFinalKing::Render(void)
{
	// If this entity is not active, then skip this
	if (bStatus == false)
	{
		return;
	}

	CSolidObject::Render();
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CFinalKing::PostRender(void)
{
	// If this entity is not active, then skip this
	if (bStatus == false)
	{
		return;
	}

	CSolidObject::PostRender();
}

/**
 @brief Calculates the front vector from the Camera's (updated) Euler Angles
 */
void CFinalKing::UpdateEnemyVectors(void)
{
	// Calculate the new vec3Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(fYaw)) * cos(glm::radians(fPitch));
	front.y = sin(glm::radians(fPitch));
	front.z = sin(glm::radians(fYaw)) * cos(glm::radians(fPitch));
	front = glm::normalize(front);

	vec3Front = front;
	// Also re-calculate the Right and Up vector
	// Normalize the vectors, because their length gets closer to 0 the more 
	// you look up or down which results in slower movement.
	vec3Right = glm::normalize(glm::cross(vec3Front, vec3WorldUp));
	vec3Up = glm::normalize(glm::cross(vec3Right, vec3Front));

	// If the camera is attached to this player, then update the camera
	if (cCamera)
	{
		cCamera->vec3Front = vec3Front;
		cCamera->vec3Right = vec3Right;
		cCamera->vec3Up = vec3Up;
	}
}

/**
 @brief Constraint the player's position
 */
void CFinalKing::Constraint(void)
{
	// Get the new height
	float fNewYValue = cTerrain->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;
	// Smooth out the change in height
	vec3Position.y += (fNewYValue - vec3Position.y) * 0.05f;
}

/**
 @brief Update Front Vector and Yaw
 */
void CFinalKing::UpdateFrontAndYaw(void)
{
	fYaw = glm::degrees(glm::acos(dot(glm::vec3(1.0f, 0.0f, 0.0f), vec3Front)));
	if (cross(glm::vec3(1.0f, 0.0f, 0.0f), vec3Front).y < 0.0f)
		fYaw *= -1;
}