/**
 CStructure3D
 By: Toh Da Jun
 Date: Apr 2020
 */
#include "Bloodbath.h"

// Include ShaderManager
#include "RenderControl/ShaderManager.h"

// Include MeshBuilder
#include "Primitives/MeshBuilder.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include <iostream>
using namespace std;

/**
 @brief Default Constructor
 */
CBloodbath::CBloodbath(void)
{
	// Set the default position to the origin
	vec3Position = glm::vec3(0.0f, fHeightOffset, 0.0f);
}

/**
 @brief Constructor with vectors
 @param vec3Position A const glm::vec3 variable which contains the position of the camera
 @param vec3Front A const glm::vec3 variable which contains the up direction of the camera
 @param yaw A const float variable which contains the yaw of the camera
 @param pitch A const float variable which contains the pitch of the camera
 */
CBloodbath::CBloodbath(	const glm::vec3 vec3Position,
							const glm::vec3 vec3Front)
{
	this->vec3Position = glm::vec3(vec3Position.x, vec3Position.y + fHeightOffset, vec3Position.z);
	this->vec3Front = vec3Front;
}

/**
 @brief Destructor
 */
CBloodbath::~CBloodbath(void)
{
}

/**
 @brief Initialise this class instance
 @return true is successfully initialised this class instance, else false
 */
bool CBloodbath::Init(void)
{
	// Call the parent's Init()
	CSolidObject::Init();

	// Set the type
	SetType(CEntity3D::TYPE::BLOODBATH);

	// Generate and bind the VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	mesh = CMeshBuilder::GenerateQuad(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// load and create a texture 
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Scene3D_Structure_01.tga", false);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene3D_Structure_01.tga" << endl;
		return false;
	}

	return true;
}


/**
 @brief Set model
 @param model A const glm::mat4 variable containing the model for this class instance
 */
void CBloodbath::SetModel(const glm::mat4 model)
{
	this->model = model;
}

/**
 @brief Set view
 @param view A const glm::mat4 variable containing the model for this class instance
 */
void CBloodbath::SetView(const glm::mat4 view)
{
	this->view = view;
}

/**
 @brief Set projection
 @param projection A const glm::mat4 variable containing the model for this class instance
 */
void CBloodbath::SetProjection(const glm::mat4 projection)
{
	this->projection = projection;
}

/**
 @brief Update the elapsed time
 @param dt A const double variable containing the elapsed time since the last frame
 @return A bool variable
 */
bool CBloodbath::Update(const double dElapsedTime)
{
	CSolidObject::Update(dElapsedTime);

	return true;
}

/**
@brief PreRender Set up the OpenGL display environment before rendering
*/
void CBloodbath::PreRender(void)
{
	CSolidObject::PreRender();
}

/**
@brief Render Render this instance
@param cShader A Shader* variable which contains the Shader to use in this class instance
*/
void CBloodbath::Render(void)
{
	CSolidObject::Render();
}

/**
@brief PostRender Set up the OpenGL display environment after rendering.
*/
void CBloodbath::PostRender(void)
{
	CSolidObject::PostRender();
}
