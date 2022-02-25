#include "Dragon.h"

 // Include ShaderManager
#include "RenderControl/ShaderManager.h"

 // Include LoadOBJ
#include "System/LoadOBJ.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include <iostream>
using namespace std;

// Set this to true if printing the debug info
#define _DEBUG_FSM false

/**
 @brief Default Constructor
 */
CDragon::CDragon(void)
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
CDragon::CDragon(const glm::vec3 vec3Position,
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

	srand(time(NULL));
}

/**
 @brief Destructor
 */
CDragon::~CDragon(void)
{
	if (cWaypointManager)
	{
		// We set it to NULL only since it was declared somewhere else
		cWaypointManager = NULL;
	}

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
bool CDragon::Init(void)
{
	// Call the parent's Init()
	CSolidObject::Init();

	// Set the type
	SetType(CEntity3D::TYPE::NPC);

	// Initialise the cPlayer3D
	cPlayer3D = CPlayer3D::GetInstance();

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<ModelVertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	std::string file_path = "Models/Dragon/Dragon.obj";
	bool success = CLoadOBJ::LoadOBJ(file_path.c_str(), vertices, uvs, normals, true);
	if (!success)
	{
		cout << "Unable to load Models/Dragon/Dragon.obj" << endl;
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
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Models/Dragon/dragontexture.tga", false);
	if (iTextureID == 0)
	{
		cout << "Unable to load Models/dragontexture.tga" << endl;
		return false;
	}

	// Store the handler to the terrain
	cTerrain = CTerrain::GetInstance();

	// Movement Control
	fMovementSpeed = 15.0f;
	iCurrentNumMovement = 0;
	iMaxNumMovement = 100;

	glm::vec3 pos(vec3Position.x, cPlayer3D->GetPosition().y, vec3Position.z);
	vec3Front = glm::normalize((cPlayer3D->GetPosition() - pos));

	patrolTime = 0;

	bIsDisplayed = false;

	// Init cWaypointManager
	cWaypointManager = new CWaypointManager;
	cWaypointManager->Init();

	// Add in some test Waypoints
	int m_iWayPointID = cWaypointManager->AddWaypoint(glm::vec3(0.0f, vec3Position.y, -30.0f));
	m_iWayPointID = cWaypointManager->AddWaypoint(m_iWayPointID, glm::vec3(30.0f, vec3Position.y, 0.0f));
	m_iWayPointID = cWaypointManager->AddWaypoint(m_iWayPointID, glm::vec3(-30.0f, vec3Position.y, 0.0f));

	cWaypointManager->PrintSelf();

	sCurrentFSM = FSM::ATTACK;

	return true;
}


/**
 @brief Set model
 @param model A const glm::mat4 variable containing the model for this class instance
 */
void CDragon::SetModel(const glm::mat4 model)
{
	this->model = model;
}

/**
 @brief Set view
 @param view A const glm::mat4 variable containing the model for this class instance
 */
void CDragon::SetView(const glm::mat4 view)
{
	this->view = view;
}

/**
 @brief Set projection
 @param projection A const glm::mat4 variable containing the model for this class instance
 */
void CDragon::SetProjection(const glm::mat4 projection)
{
	this->projection = projection;
}

/**
 @brief Attach a camera to this class instance
 @param cCamera A CCamera* variable which contains the camera
 */
void CDragon::AttachCamera(CCamera* cCamera)
{
	this->cCamera = cCamera;

	// Update the camera's attributes with the enemy's attributes
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
bool CDragon::IsCameraAttached(void)
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
void CDragon::SetWeapon(const int iSlot, CWeaponInfo* cWeaponInfo)
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
CWeaponInfo* CDragon::GetWeapon(void) const
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
void CDragon::SetCurrentWeapon(const int iSlot)
{
	iCurrentWeapon = iSlot;
}

/**
 @brief Discharge weapon
 @return A bool value
 */
bool CDragon::DischargeWeapon(void) const
{
	glm::vec3 pos(vec3Position.x, vec3Position.y + 4.7, vec3Position.z);
	glm::vec3 target = glm::normalize(cPlayer3D->GetPosition() - pos);

	if ((iCurrentWeapon == 0) && (cPrimaryWeapon))
	{
		return cPrimaryWeapon->Discharge(pos, target, (CSolidObject*)this);
	}
	else if ((iCurrentWeapon == 1) && (cSecondaryWeapon))
	{
		return cSecondaryWeapon->Discharge(pos, vec3Front, (CSolidObject*)this);
	}
}

/**
 @brief Processes input received from any keyboard-like input system as enemy movements. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
 @param direction A const Enemy_Movement variable which contains the movement direction of the camera
 @param deltaTime A const float variable which contains the delta time for the realtime loop
 */
void CDragon::ProcessMovement(const ENEMYMOVEMENT direction, const float deltaTime)
{
	float velocity = fMovementSpeed * deltaTime;
	vec3Position += vec3Front * velocity;

	if (cCamera)
	{
		cCamera->vec3Position = vec3Position;
	}
}

/**
 @brief Processes input received from a mouse input system as enemy rotation. Expects the offset value in both the x and y direction.
 @param xoffset A const float variable which contains the x axis of the mouse movement
 */
void CDragon::ProcessRotate(const float fXOffset)
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
bool CDragon::Update(const double dElapsedTime)
{
	// Don't update if this entity is not active
	if (bStatus == false)
	{
		return false;
	}

	// Store the enemy's current position, if rollback is needed
	StorePositionForRollback();

	// Offset centre position
	glm::vec3 pos(vec3Position.x, cPlayer3D->GetPosition().y, vec3Position.z);
	glm::vec3 targetFront = glm::normalize((cPlayer3D->GetPosition() - pos));
	pos += 5.5f * targetFront;

	// Calculate angle from player
	targetFront = glm::normalize((cPlayer3D->GetPosition() - pos));
	float dot = glm::dot(targetFront, vec3Front);
	//cout << dot << endl;

	if (dot < 0 && sCurrentFSM == FSM::ATTACK)
	{
		sCurrentFSM = FSM::IDLE;
	}
	else if (dot >= 0.9 && sCurrentFSM == FSM::ATTACK)
	{
		vec3Front = targetFront;
		DischargeWeapon();

		// Process the movement
		ProcessMovement(ENEMYMOVEMENT::FORWARD, (float)dElapsedTime);
	}
	else if (dot >= 0 && sCurrentFSM == FSM::ATTACK)
	{
		// Slowly turn left or right based on player position
		if (glm::dot(vec3Right, targetFront) < 0)
		{
			double angle = -5 * dElapsedTime;
			vec3Front = glm::mat3(cos(angle), 0, sin(angle), 0, 1, 0, -sin(angle), 0, cos(angle)) * vec3Front;
		}
		else
		{
			double angle = 5 * dElapsedTime;
			vec3Front = glm::mat3(cos(angle), 0, sin(angle), 0, 1, 0, -sin(angle), 0, cos(angle)) * vec3Front;
		}
	}
	else if (sCurrentFSM == FSM::IDLE)
	{
		// Process the movement
		ProcessMovement(ENEMYMOVEMENT::FORWARD, (float)dElapsedTime);

		if (vec3Position.x <= cTerrain->GetMinPos().x * 1.1 || vec3Position.x >= cTerrain->GetMaxPos().x * 1.1)
		{
			sCurrentFSM = FSM::PATROL;
			patrolTime = rand() % 3 + 2;

			if (rand() % 2 == 0)
			{
				vec3Front = glm::vec3(0, 0, -1);
			}
			else
			{
				vec3Front = glm::vec3(0, 0, 1);
			}
		}
		else if (vec3Position.z <= cTerrain->GetMinPos().z * 1.1 || vec3Position.z >= cTerrain->GetMaxPos().z * 1.1)
		{
			sCurrentFSM = FSM::PATROL;
			patrolTime = rand() % 3 + 2;

			if (rand() % 2 == 0)
			{
				vec3Front = glm::vec3(-1, 0, 0);
			}
			else
			{
				vec3Front = glm::vec3(1, 0, 0);
			}
		}
	}
	else if (sCurrentFSM == FSM::PATROL)
	{
		patrolTime -= dElapsedTime;
		if (patrolTime <= 0)
		{
			sCurrentFSM = FSM::ATTACK;
			vec3Front = targetFront;
		}
		else
		{
			// Process the movement
			ProcessMovement(ENEMYMOVEMENT::FORWARD, (float)dElapsedTime);

			if (vec3Position.x <= cTerrain->GetMinPos().x && vec3Position.z <= cTerrain->GetMinPos().z)
			{
				if (glm::dot(vec3Right, -vec3Position) < 0)
				{

				}
				else
				{

				}
			}
		}
	}
	UpdateFrontAndYaw();

	// Update the model
	model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	model = glm::translate(model, vec3Position);
	model = glm::scale(model, vec3Scale);
	model = glm::rotate(model, glm::radians(fYaw), glm::vec3(0.0f, 1.0f, 0.0f));

	// Update the weapon's position
	if (cPrimaryWeapon)
	{
		//cPrimaryWeapon->SetPosition(vec3Position + glm::vec3(0.05f, -0.075f, 0.5f));
		cPrimaryWeapon->Update(dElapsedTime);
		glm::mat4 gunModel = model;
		gunModel = glm::rotate(gunModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		gunModel = glm::translate(gunModel, glm::vec3(0.05f, -0.075f, 0.5f));
		cPrimaryWeapon->SetModel(gunModel);
	}
	if (cSecondaryWeapon)
	{
		cSecondaryWeapon->SetPosition(vec3Position + glm::vec3(0.05f, -0.075f, 0.5f));
		cSecondaryWeapon->SetRotation(fYaw, glm::vec3(0.0f, 1.0f, 0.0f));
		cSecondaryWeapon->Update(dElapsedTime);
	}

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CDragon::PreRender(void)
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
void CDragon::Render(void)
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
void CDragon::PostRender(void)
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
void CDragon::UpdateEnemyVectors(void)
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

	if (cCamera)
	{
		cCamera->vec3Front = vec3Front;
		cCamera->vec3Right = vec3Right;
		cCamera->vec3Up = vec3Up;
	}
}

/**
 @brief Constraint the enemy's position
 */
void CDragon::Constraint(void)
{
	// Get the new height
	float fNewYValue = cTerrain->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;
	// Smooth out the change in height
	vec3Position.y += (fNewYValue - vec3Position.y) * 0.05f;
}

/**
 @brief Update Front Vector and Yaw
 */
void CDragon::UpdateFrontAndYaw(void)
{
	fYaw = glm::degrees(glm::acos(dot(glm::vec3(1.0f, 0.0f, 0.0f), vec3Front)));
	if (cross(glm::vec3(1.0f, 0.0f, 0.0f), vec3Front).y < 0.0f)
		fYaw *= -1;
}
