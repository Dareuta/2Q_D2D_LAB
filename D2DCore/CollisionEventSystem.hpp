//========================================================================
// CollisionEventSystem.hpp
//========================================================================

#pragma once

#include "pch.h"
#include "GameObject.hpp"
#include "CollisionFlagComponent.hpp"

class CollisionEventSystem {
public:
	void Update(const std::vector<std::shared_ptr<GameObject>>& objects) {
		for (auto& obj : objects) {
			auto flag = obj->GetComponent<CollisionFlagComponent>();
			if (!flag) continue;

			for (GameObject* target : flag->currentHits) {
				if (flag->previousHits.find(target) != flag->previousHits.end())
					flag->onStay.Broadcast(target);
				else
					flag->onEnter.Broadcast(target);
			}

			for (GameObject* target : flag->previousHits) {
				if (flag->currentHits.find(target) == flag->currentHits.end())
					flag->onExit.Broadcast(target);
			}

			flag->Sync();
		}
	}
};
