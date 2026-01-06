//========================================================================
// CollisionSystem.hpp
//========================================================================

#pragma once

#include "pch.h"
#include <vector>
#include "GameObject.hpp"
#include "TransformComponent.hpp"
#include "ColliderComponent.hpp"
#include "CollisionFlagComponent.hpp"

class CollisionSystem {
public:
	struct CollisionPair {
		GameObject* a;
		GameObject* b;
	};
private:
	std::vector<CollisionPair> collisions;

	bool AABBOverlap(const D2D1_RECT_F& a, const D2D1_RECT_F& b) const {
		return !(a.right < b.left || a.left > b.right ||
			a.bottom < b.top || a.top > b.bottom); // AABB검사
	}

public:
	void Update(const std::vector<std::shared_ptr<GameObject>>& objects) {
		collisions.clear(); // 일단 비우고

		for (size_t i = 0; i < objects.size(); ++i) {
			// 페어 a에 대한 유효성
			auto a = objects[i];
			auto ca = a->GetComponent<ColliderComponent>();
			auto ta = a->GetComponent<TransformComponent>();
			if (!ca || !ta) continue;


			// 페어 b에 대한 유효성, 신기하게도 모든 경우에수가 다 나옴			
			for (size_t j = i + 1; j < objects.size(); ++j) {
				auto b = objects[j];
				auto cb = b->GetComponent<ColliderComponent>();
				auto tb = b->GetComponent<TransformComponent>();
				if (!cb || !tb) continue;
				
				if (ca->isStatic && cb->isStatic) continue; // 둘다 고정이면 건너뜀

				D2D1_RECT_F rectA = ca->GetAABB(ta->position);
				D2D1_RECT_F rectB = cb->GetAABB(tb->position);

				bool isTriggerPair = ca->isTrigger || cb->isTrigger; // 둘중에 하나라도 trigger여야 이벤트 발생함

				if (AABBOverlap(rectA, rectB)) {
					collisions.push_back({ a.get(), b.get() }); // 충돌이 발생한것들은 백터에 담아둠, 나중에 빼서 처리함

					if (isTriggerPair) {
						auto fa = a->GetComponent<CollisionFlagComponent>();
						auto fb = b->GetComponent<CollisionFlagComponent>();

						if (fa) fa->currentHits.insert(b.get()); // a에 b랑 부딪힘 기록
						if (fb) fb->currentHits.insert(a.get()); // b에 a랑 부딪힘 기록
					}
				}
			}
		}
	}

	const std::vector<CollisionPair>& GetCollisions() const {
		return collisions;
	}
};