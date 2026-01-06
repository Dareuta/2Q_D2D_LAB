//========================================================================
// TransformSystem.hpp
//========================================================================

#pragma once

#include <vector>

#include "pch.h"
#include "GameObject.hpp"
#include "TransformComponent.hpp"

class TransformSystem {
public:
	void Update(const std::vector<std::shared_ptr<GameObject>>& objects) {
		for (auto obj : objects) {
			auto tf = obj->GetComponent<TransformComponent>();
			if (tf) {
				tf->GetWorldMatrix(); // dirty플래그 있으면 알아서 초기화, 없으면 넘어감
			}
		}
	}
};