//========================================================================
// KinematicMovementSystem.hpp
//========================================================================

#pragma once

#include "pch.h"
#include "GameObject.hpp"
#include "KinematicBodyComponent.hpp"
#include "TransformComponent.hpp"
#include "TimeSystem.hpp"

class KinematicMovementSystem {
public:
	void Update(const std::vector<std::shared_ptr<GameObject>>& objects) {
		float dt = TimeSystem::GetInstance().GetDelta(); // 업데이트 내부에서 델타 넘기는 부분 있으니까, 나중에 생각해봐야함

		for (auto& obj : objects) {
			auto body = obj->GetComponent<KinematicBodyComponent>();
			auto tf = obj->GetComponent<TransformComponent>();
			if (!body || !tf) continue;

			// 단위 벡터에 스칼라 곱해서, 이동시키는거임
			tf->position.x += body->direction.x * body->speed * dt;
			tf->position.y += body->direction.y * body->speed * dt;
		}
	}
};
