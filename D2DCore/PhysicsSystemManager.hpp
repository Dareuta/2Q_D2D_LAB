//========================================================================
// PhysicsSystemManager.hpp
//========================================================================

#pragma once

#include "pch.h"
#include <vector>
#include <memory>
#include "GameObject.hpp"

#include "TransformSystem.hpp"
#include "DynamicMovementSystem.hpp"
#include "KinematicMovementSystem.hpp"
#include "CollisionSystem.hpp"
#include "CollisionResolverSystem.hpp"
#include "CollisionEventSystem.hpp"

class PhysicsSystemManager {
private:
	TransformSystem* transformSystem = nullptr;
	DynamicMovementSystem* dynamicMovement = nullptr;
	KinematicMovementSystem* kinematicMovement = nullptr;
	CollisionSystem* collisionSystem = nullptr;
	CollisionResolverSystem* collisionResolver = nullptr;
	CollisionEventSystem* collisionEvent = nullptr;

public:
	void SetTransformSystem(TransformSystem* s) { transformSystem = s; }
	void SetDynamicMovementSystem(DynamicMovementSystem* s) { dynamicMovement = s; }
	void SetKinematicMovementSystem(KinematicMovementSystem* s) { kinematicMovement = s; }
	void SetCollisionSystem(CollisionSystem* s) { collisionSystem = s; }
	void SetCollisionResolverSystem(CollisionResolverSystem* s) { collisionResolver = s; }
	void SetCollisionEventSystem(CollisionEventSystem* s) { collisionEvent = s; }

	void Update(const std::vector<std::shared_ptr<GameObject>>& objects) {
		if (transformSystem) transformSystem->Update(objects);
		if (dynamicMovement) dynamicMovement->Update(objects);
		if (kinematicMovement) kinematicMovement->Update(objects);
		if (collisionSystem) collisionSystem->Update(objects);
		if (collisionResolver) collisionResolver->Resolve(collisionSystem->GetCollisions());
		if (collisionEvent) collisionEvent->Update(objects);
	}
};
