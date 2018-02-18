#ifndef MODULEPHYSICS_H_
#define MODULEPHYSICS_H_

#include "Module.h"

#include <map>

#define DEBUG_PHYSX_DT 0.000000000001

class jpPhysicsWorld;
class jpPhysicsRigidBody;

namespace physx 
{
	class PxPhysics;
	class PxScene;
	class PxSceneDesc;
	class PxRigidActor;
	class PxRigidStatic;
	class PxRigidDynamic;
	class PxBounds3;
	class PxSimulationEventCallback;
}

class ModulePhysics : public Module
{
public:
	ModulePhysics(bool start_enabled = true);
	~ModulePhysics();

	bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	//update_status Update(float dt);
	update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool SaveConfig(JSON_Object* node);
	bool CleanUp();

	bool SetEventListenrs();
	void OnEvent(Event& event);

	// PhysX Methods ----------------
	jpPhysicsRigidBody* GetNewRigidBody(Component* component, bool dynamic = false);

	void ChangeRigidActorToStatic(jpPhysicsRigidBody* actor, Component* comp);
	void ChangeRigidActorToDynamic(jpPhysicsRigidBody* actor, Component* comp);

	// Collision Callback -----------
	void OnTrigger(physx::PxRigidActor* trigger, physx::PxRigidActor* actor, JP_COLLISION_TYPE type);

	// Debug Methods ----------------
	void DrawPhysics();

private:
	physx::PxPhysics* mPhysics = nullptr;

	jpPhysicsWorld * physics_world = nullptr;
	physx::PxScene* mScene = nullptr;

	// Config data ------------------
	// Use only as a debug tool -----
	bool render_physics = false;
	bool render_on_play = false;

	std::map<physx::PxRigidActor*, Component*> colliders;
};


#endif // !MODULEPHYSICS_H_