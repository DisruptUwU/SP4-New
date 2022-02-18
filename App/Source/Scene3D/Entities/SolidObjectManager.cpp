/**
 CSolidObjectManager
 By: Toh Da Jun
 Date: Apr 2020
 */

#include "SolidObjectManager.h"

 // Include CCollisionManager
#include "Primitives/CollisionManager.h"

// Include CCameraEffectsManager
#include "../CameraEffects/CameraEffectsManager.h"

#include <iostream>

using namespace std;

/**
 @brief Default Constructor
 */
CSolidObjectManager::CSolidObjectManager(void)
	: model(glm::mat4(1.0f))
	, view(glm::mat4(1.0f))
	, projection(glm::mat4(1.0f))
	, cProjectileManager(NULL)
{
}

/**
 @brief Destructor
 */
CSolidObjectManager::~CSolidObjectManager(void)
{
	// We won't delete this since it was created elsewhere
	cProjectileManager = NULL;

	// Remove all CSolidObject
	std::list<CSolidObject*>::iterator it = lSolidObject.begin(), end = lSolidObject.end();
	while (it != end)
	{
		// Delete if done
		delete* it;
		it = lSolidObject.erase(it);
	}
}

/**
 @brief Initialise this class instance
 @return A bool variable
 */
bool CSolidObjectManager::Init(void)
{
	lSolidObject.clear();

	cProjectileManager = CProjectileManager::GetInstance();

	cPlayer3D = CPlayer3D::GetInstance();

	//cFinalBoss3D = CFinalBoss3D::GetInstance();

	enemy_lvl1_count = 4; // Set this to number of enemies in lvl 1

	return true;
}

/**
 @brief Set model
 @param model A const glm::mat4 variable containing the model for this class instance
 */
void CSolidObjectManager::SetModel(const glm::mat4 model)
{
	this->model = model;
}

/**
 @brief Set view
 @param view A const glm::mat4 variable containing the model for this class instance
 */
void CSolidObjectManager::SetView(const glm::mat4 view)
{
	this->view = view;
}

/**
 @brief Set projection
 @param projection A const glm::mat4 variable containing the model for this class instance
 */
void CSolidObjectManager::SetProjection(const glm::mat4 projection)
{
	this->projection = projection;
}

/**
 @brief Add a CSolidObject* to this class instance
 @param cSolidObject The CSolidObject* variable to be added to this class instance
 */
void CSolidObjectManager::Add(CSolidObject* cSolidObject)
{
	lSolidObject.push_back(cSolidObject);
}

/**
 @brief Remove a CSolidObject* from this class instance
 @param cSolidObject The CSolidObject* variable to be erased from this class instance
 @return A bool variable
 */
bool CSolidObjectManager::Erase(CSolidObject* cSolidObject)
{
	// Find the entity's iterator
	std::list<CSolidObject*>::iterator findIter = std::find(lSolidObject.begin(), lSolidObject.end(), cSolidObject);

	// Delete the entity if found
	if (findIter != lSolidObject.end())
	{
		// Delete the CSolidObject
		//delete *findIter;
		// Go to the next iteration after erasing from the list
		findIter = lSolidObject.erase(findIter);
		return true;
	}
	// Return false if not found
	return false;
}

/**
 @brief Collision Check for a CSolidObject*
 @param cSolidObject The CSolidObject* variable to be checked
 @return A bool variable
 */
bool CSolidObjectManager::CollisionCheck(CSolidObject* cSolidObject)
{
	std::list<CSolidObject*>::iterator it, end;
	end = lSolidObject.end();
	for (it = lSolidObject.begin(); it != end; ++it)
	{
		// Check for collisions between the 2 entities
		if (CCollisionManager::BoxBoxCollision(cSolidObject->GetPosition() + cSolidObject->boxMin,
			cSolidObject->GetPosition() + cSolidObject->boxMax,
			(*it)->GetPosition() + (*it)->boxMin,
			(*it)->GetPosition() + (*it)->boxMax) == true)
		{
			// Rollback the cSolidObject's position
			cSolidObject->RollbackPosition();
			// Rollback the NPC's position
			(*it)->RollbackPosition();
			if ((*it)->GetType() == CSolidObject::TYPE::NPC)
				cout << "** Collision between this Entity and a NPC ***" << endl;
			else if ((*it)->GetType() == CSolidObject::TYPE::OTHERS)
				cout << "** Collision between this Entity and an OTHERS ***" << endl;
			else if ((*it)->GetType() == CSolidObject::TYPE::STRUCTURE)
				cout << "** Collision between this Entity and a STRUCTURE ***" << endl;
			else if ((*it)->GetType() == CSolidObject::TYPE::PROJECTILE)
				cout << "** Collision between this Entity and a PROJECTILE ***" << endl;
			// Quit this loop since a collision has been found
			break;
		}
	}

	return true;
}

/**
 @brief Update the elapsed time
 @param dt A const double variable containing the elapsed time since the last frame
 @return A bool variable
 */
bool CSolidObjectManager::Update(const double dElapsedTime)
{
	std::list<CSolidObject*>::iterator it, end;

	// Update all CSolidObject
	end = lSolidObject.end();
	for (it = lSolidObject.begin(); it != end; ++it)
	{
		(*it)->Update(dElapsedTime);
	}

	return true;
}

/**
 @brief Collision Check for all entities and projectiles
 @return A bool variable
 */
bool CSolidObjectManager::CheckForCollision(void)
{
	bool bResult = false;
	//DeadEnemies = 0;

	std::list<CSolidObject*>::iterator it, end;
	std::list<CSolidObject*>::iterator it_other;

	// Check for collisions among entities
	end = lSolidObject.end();
	for (it = lSolidObject.begin(); it != end; ++it)
	{
		// If the entity is not active, then skip it (Unless entity is door for lvl 1, which will activate upon killing of all four enemies in lvl 1)
		if ((*it)->GetStatus() == false)
		{
			if (enemy_lvl1_count <= 0 && (*it)->GetType() == CEntity3D::TYPE::DOOR)
			{
				(*it)->SetStatus(true);
				cout << "** Level 1 portal activated ***" << endl;
				//continue;
			}
			continue; // Go to next entity in loop
		}

		// Set door to false first
		if ((*it)->GetType() == CSolidObject::TYPE::DOOR && enemy_lvl1_count > 0)
		{
			cout << "** Level 1 portal set false ***" << endl;
			(*it)->SetStatus(false);
		}
			

		for (it_other = lSolidObject.begin(); it_other != end; ++it_other)
		{
			// If the entity is not active, then skip it
			if ((*it_other)->GetStatus() == false)
				continue;

			// If the 2 entities to check are the same, then skip this iteration
			if (it_other == it)
				continue;

			// Check for collisions between the 2 entities
			if (CCollisionManager::BoxBoxCollision((*it)->GetPosition() + (*it)->boxMin,
				(*it)->GetPosition() + (*it)->boxMax,
				(*it_other)->GetPosition() + (*it_other)->boxMin,
				(*it_other)->GetPosition() + (*it_other)->boxMax) == true)
			{
				// Check if a movable entity collides with another movable entity
				if (
					((*it)->GetType() == CSolidObject::TYPE::PLAYER)
					&&
					(((*it_other)->GetType() >= CSolidObject::TYPE::NPC) &&
						((*it_other)->GetType() <= CSolidObject::TYPE::OTHERS))
					)
				{
					(*it)->RollbackPosition();
					(*it_other)->RollbackPosition();
					bResult = true;
					cout << "** Collision between Player and an Entity ***" << endl;
					break;
				}
				else if (
					(((*it)->GetType() >= CSolidObject::TYPE::NPC) &&
						((*it)->GetType() <= CSolidObject::TYPE::OTHERS))
					&&
					(((*it_other)->GetType() >= CSolidObject::TYPE::NPC) &&
						((*it_other)->GetType() <= CSolidObject::TYPE::OTHERS))
					)
				{
					(*it)->RollbackPosition();
					(*it_other)->RollbackPosition();
					cout << "** Collision between 2 Entities ***" << endl;
					break;
				}
				// Check if a movable entity collides with a non-movable entity
				if (
					(((*it)->GetType() >= CSolidObject::TYPE::PLAYER) &&
						((*it)->GetType() <= CSolidObject::TYPE::OTHERS))
					&&
					((*it_other)->GetType() == CSolidObject::TYPE::STRUCTURE)
					)
				{
					(*it)->RollbackPosition();
					if (((*it)->GetType() == CSolidObject::TYPE::PLAYER))
						//bResult = true;
					cout << "** Collision between Entity and Structure ***" << endl;
					break;
				}

				if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::DOOR))
				{
					wenttodoor = true;
					cout << "** teleporting! ***" << endl;
					break;
				}

				if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::DOORLVL2))
				{
					Doorlevel2 = true;
					cout << "** teleporting to level 2 ***" << endl;
					break;
				}

				if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::SOUL))
				{
					cPlayer3D->healthdownbyhydra = true;
					(*it)->RollbackPosition();
					(*it_other)->RollbackPosition();
					break;
				}

				if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::FINALBOSS))
				{
					(*it)->RollbackPosition();
					(*it_other)->RollbackPosition();
					break;
				}

				if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::SPEEDPOWER) /*&& CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_F)*/)
				{
					cPlayer3D->speedPower = true;
					(*it_other)->RollbackPosition();
					(*it_other)->SetStatus(false);
					cout << "** speed ***" << endl;
					break;
				}

				if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::DEFPOWER) /*&& CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_F)*/)
				{
					cPlayer3D->DefenceIncrease = true;
					(*it_other)->RollbackPosition();
					(*it_other)->SetStatus(false);
					cout << "** Trapped ***" << endl;
					break;
				}

				if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::DMGPOWER) /*&& CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_F)*/)
				{
					cPlayer3D->AtkIncrease = true;
					(*it_other)->RollbackPosition();
					(*it_other)->SetStatus(false);
					cout << "** Trapped ***" << endl;
					break;
				}

				if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::BEARTRAP) /*&& CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_F)*/)
				{
					cPlayer3D->trapped = true;
					(*it_other)->RollbackPosition();
					(*it_other)->SetStatus(false);
					cout << "** Trapped ***" << endl;
					break;
				}

				if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::JUMPSCARETRAP) /*&& CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_F)*/)
				{
					cPlayer3D->jumpscaretrapped = true;
					(*it_other)->RollbackPosition();
					(*it_other)->SetStatus(false);
					cout << "** Trapped ***" << endl;
					break;
				}

				if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::HEALTHPOWER) /*&& CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_F)*/)
				{
					cPlayer3D->healthPower = true;
					(*it_other)->RollbackPosition();
					(*it_other)->SetStatus(false);
					cout << "** health ***" << endl;
					break;
				}

			}
		}
	}

	// Check for collisions between entities and projectiles
	end = lSolidObject.end();
	for (it = lSolidObject.begin(); it != end; ++it)
	{
		// If the entity is not active, then skip it
		if ((*it)->GetStatus() == false)
			continue;

		for (unsigned int i = 0; i < cProjectileManager->vProjectile.size(); i++)
		{
			// If the entity is not active, then skip it
			if ((cProjectileManager->vProjectile[i])->GetStatus() == false)
				continue;

			// Use ray tracing to check for collisions between the 2 entities
			if (CCollisionManager::RayBoxCollision((*it)->GetPosition() + (*it)->boxMin,
				(*it)->GetPosition() + (*it)->boxMax,
				(cProjectileManager->vProjectile[i])->GetPreviousPosition(),
				(cProjectileManager->vProjectile[i])->GetPosition()))
			{
				if ((*it)->GetType() == CSolidObject::TYPE::PLAYER)
				{
					// If this projectile is fired by the player, then skip it
					if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
						continue;
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** RayBoxCollision between Player and Projectile ***" << endl;
					bResult = true;
					if (moreaggresivepart2 == true) //if (healthbelow50 == true)
					{
						cPlayer3D->healthdownbyhydramore = true;
					}
					else
					{
						cPlayer3D->healthdownbyhydra = true;
					}
					break;
				}
				//else if ((((*it)->GetType() == CSolidObject::TYPE::PLAYER)) && ((*it_other)->GetType() == CSolidObject::TYPE::HYDRA))
				//{
				//	// If this projectile is fired by the player, then skip it
				//	if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
				//		continue;
				//	(cProjectileManager->vProjectile[i])->SetStatus(false);
				//	cout << "** RayBoxCollision between Player and Hydra Projectile ***" << endl;
				//	cPlayer3D->healthdownbyhydra = true;
				//	break;
				//}
				else if ((*it)->GetType() == CSolidObject::TYPE::NPC)
				{
					// If this projectile is fired by the NPC, then skip it
					if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
						continue;
					(*it)->SetStatus(false);
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					DeadEnemies += 1;
					cout << "** RayBoxCollision between Enemy and Projectile ***" << endl;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::ENEMY_LVL1)
				{
					// If this projectile is fired by the NPC, then skip it
					if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
						continue;
					(*it)->SetStatus(false);
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					enemy_lvl1_count--; // We use a hardcoded value for now for MVP
					cout << "** RayBoxCollision between lvl 1 enemy and Projectile ***" << endl;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::SOUL)
				{
					// If this projectile is fired by the NPC, then skip it
					if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
						continue;
					(*it)->SetStatus(false);
					cFinalBoss3D->soulsAlive--;
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** RayBoxCollision between NPC and Projectile ***" << endl;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::FINALHEALBOSS)
				{
					// If this projectile is fired by the NPC, then skip it
					if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
						continue;
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					(*it)->SetStatus(false);
					cFinalBoss3D->healersAlive--;
					cout << "** RayBoxCollision between Healer and Projectile ***" << endl;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::FINALBOSS)
				{
					// If this projectile is fired by the NPC, then skip it
					if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
						continue;
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					if (cFinalBoss3D->phase <= 0)
					{

					}
					else
					{
						cFinalBoss3D->FinalBossHp -= cPlayer3D->Damage;
					}

					if (cFinalBoss3D->FinalBossHp <= 0) {
						cFinalBoss3D->KilledFinalBoss = true;
						(*it)->SetStatus(false);
					}

					cout << "** RayBoxCollision between NPC and Projectile ***" << endl;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::HYDRA)
				{
					// If this projectile is fired by the NPC, then skip it
					if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
						continue;
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					/*if (cHydra->moreaggresivepart2 == true)
					{
						cHydra->HydraBossHp = 70;
					}*/
					if (cHydra->changingform == true)
					{
						
					}
					else 
					{
						cHydra->HydraBossHp -= cPlayer3D->Damage;
					}
					if (cHydra->HydraBossHp <= 0) {
						HydraKilled = true;
						(*it)->SetStatus(false);
					}
					cout << "** RayBoxCollision between NPC and Projectile ***" << endl;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::DEMON)
				{
					// If this projectile is fired by the NPC, then skip it
					if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
						continue;
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cDemon->DemonHp -= 25;
					if (cDemon->DemonHp <= 0) {
						DemonKilled = true;
						(*it)->SetStatus(false);
					}
					cout << "** RayBoxCollision between Demon and Projectile ***" << endl;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::STRUCTURE)
				{
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** RayBoxCollision between Structure and Projectile ***" << endl;
					break;
				}
			}

			// Check for collisions between the 2 entities
			if (CCollisionManager::BoxBoxCollision((*it)->GetPosition() + (*it)->boxMin,
				(*it)->GetPosition() + (*it)->boxMax,
				(cProjectileManager->vProjectile[i])->GetPosition() + (cProjectileManager->vProjectile[i])->boxMin,
				(cProjectileManager->vProjectile[i])->GetPosition() + (cProjectileManager->vProjectile[i])->boxMax) == true)
			{
				if ((*it)->GetType() == CSolidObject::TYPE::PLAYER)
				{
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** BoxBoxCollision between Player and Projectile ***" << endl;
					bResult = true;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::NPC)
				{
					(*it)->SetStatus(false);
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** BoxBoxCollision between NPC and Projectile ***" << endl;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::STRUCTURE)
				{
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** BoxBoxCollision between Structure and Projectile ***" << endl;
					break;
				}
			}
		}
	}

	if (bResult == true)
	{
		CCameraEffectsManager::GetInstance()->Get("BloodScreen")->SetStatus(true);
	}

	return true;
}

/**
 @brief CleanUp all CEntity3Ds which are marked for deletion
 */
void CSolidObjectManager::CleanUp(void)
{
	std::list<CSolidObject*>::iterator it, end;
	it = lSolidObject.begin();
	end = lSolidObject.end();
	while (it != end)
	{
		if ((*it)->IsToDelete())
		{
			// Delete the CSolidObject
			delete* it;
			// Go to the next iteration after erasing from the list
			it = lSolidObject.erase(it);
		}
		else
		{
			// Go to the next iteration
			++it;
		}
	}
}

/**
 @brief Render this class instance
 */
void CSolidObjectManager::Render(void)
{
	// Render all entities
	std::list<CSolidObject*>::iterator it, end;
	end = lSolidObject.end();
	for (it = lSolidObject.begin(); it != end; ++it)
	{
		(*it)->SetView(view);
		(*it)->SetProjection(projection);
		(*it)->PreRender();
		(*it)->Render();
		(*it)->PostRender();
	}
}
