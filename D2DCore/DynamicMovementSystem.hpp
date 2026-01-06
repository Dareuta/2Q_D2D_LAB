//========================================================================
// DynamicMovementSystem.hpp
//========================================================================

#pragma once

#include "pch.h"
#include "GameObject.hpp"
#include "TransformComponent.hpp"
#include "TimeSystem.hpp"
#include "DynamicBodyComponent.hpp"

class DynamicMovementSystem {
public:
	void Update(const std::vector<std::shared_ptr<GameObject>>& objects) {
		float dt = TimeSystem::GetInstance().GetDelta();

		for (auto& obj : objects) {
			auto body = obj->GetComponent<DynamicBodyComponent>();
			auto tf = obj->GetComponent<TransformComponent>();
			if (!body || !tf) continue;

			if (body->useGravity) { // 중력 적용 여부, 즉 지상인지 여부라 생각하면 될듯
				body->acceleration.y += 980.0f * body->gravityScale; // 중력에 얼마나 영향을 받을지 스케일 값 곱해서 낙하
			}		

			body->velocity.x += body->acceleration.x * dt; // 가속이 얼마나 되었는지 dt 곱해서 구한 뒤
			body->velocity.y += body->acceleration.y * dt;

			tf->position.x += body->velocity.x * dt; // 속력만큼 이동함
			tf->position.y += body->velocity.y * dt;

			body->acceleration = { 0.0f, 0.0f }; // 가속도(addforce로 추가된) 초기화
		}
	}
};