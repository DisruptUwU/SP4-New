/**
 CBloodScreen
 By: Toh Da Jun
 Date: Apr 2020
 */
#include "Jumpscare.h"

// Include GLEW
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#endif

// Include ImageLoader
#include "System\ImageLoader.h"

// Include MeshBuilder
#include "Primitives/MeshBuilder.h"

#include <iostream>
using namespace std;

/**
 @brief Default Constructor
 */
CJumpscare::CJumpscare(void)
{
}

/**
 @brief Destructor
 */
CJumpscare::~CJumpscare(void)
{
}

/**
 @brief Initialise this class instance
 @return true is successfully initialised this class instance, else false
 */
bool CJumpscare::Init(int type)
{
	// Call the parent's Init()
	CCameraEffects::Init();

	// Set the type
	SetType(CEntity3D::TYPE::OTHERS);

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Generate and bind the VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Generate the mesh
	mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), 2.0f, 2.0f);

	// load and create a texture 
	if (type == 1)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/GUI/jumpscare.jpeg", false);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/GUI/youlose.png" << endl;
			return false;
		}
	}

	// load and create a texture 
	if (type == 2)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/GUI/jumpscare2.jpg", false);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/GUI/youlose.png" << endl;
			return false;
		}
	}

	// load and create a texture 
	if (type == 3)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/GUI/jumpscare3.jpg", false);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/GUI/youlose.png" << endl;
			return false;
		}
	}

	// load and create a texture 
	if (type == 4)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/GUI/jumpscare4.png", false);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/GUI/youlose.png" << endl;
			return false;
		}
	}

	// load and create a texture 
	if (type == 5)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/GUI/jumpscare5.jpg", false);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/GUI/youlose.png" << endl;
			return false;
		}
	}

	// load and create a texture 
	if (type == 6)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/GUI/Loadingscreen.jpg", false);
		if (iTextureID == 0)
		{
			cout << "Unable to load Image/GUI/youlose.png" << endl;
			return false;
		}
	}

	return true;
}
