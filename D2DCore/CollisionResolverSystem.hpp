//========================================================================
// CollisionResolverSystem.hpp
//========================================================================

#pragma once

#include "CollisionSystem.hpp"
#include <cmath>


class CollisionResolverSystem {
public:
	D2D1_POINT_2F GetMTV(const D2D1_RECT_F& a, const D2D1_RECT_F& b) { // 나중에 필요하면 분리해서 재활용가능한 함수임
		float dx1 = b.right - a.left; //얼마나 파고들어서 겹쳐져있는지 찾는 과정임
		float dx2 = a.right - b.left;
		float dy1 = b.bottom - a.top;
		float dy2 = a.bottom - b.top;

		float dx = (dx1 < dx2) ? dx1 : -dx2;
		float dy = (dy1 < dy2) ? dy1 : -dy2;

		if (std::abs(dx) < std::abs(dy)) // X Y축 중에, 어느쪽으로 나가야 최소한인지 비교하는거임
			return D2D1::Point2F(dx, 0.0f);
		else
			return D2D1::Point2F(0.0f, dy);
	}

	void Resolve(const std::vector<CollisionSystem::CollisionPair>& collisions) {
		for (const auto& pair : collisions) {
			auto a = pair.a;
			auto b = pair.b;

			auto ca = a->GetComponent<ColliderComponent>();
			auto cb = b->GetComponent<ColliderComponent>();
			if (!ca || !cb) continue;

			if (ca->isTrigger || cb->isTrigger) continue; // 둘중에 하나라도 트리거면 처리안함.
			// 둘다 트리거 false인 경우에만

			auto ta = a->GetComponent<TransformComponent>();
			auto tb = b->GetComponent<TransformComponent>();
			if (!ta || !tb) continue;

			D2D1_RECT_F rectA = ca->GetAABB(ta->position);
			D2D1_RECT_F rectB = cb->GetAABB(tb->position);

			auto bodyA = a->GetComponent<DynamicBodyComponent>();
			auto bodyB = b->GetComponent<DynamicBodyComponent>();

			D2D1_POINT_2F mtv = GetMTV(rectA, rectB);

			bool aStatic = ca->isStatic;
			bool bStatic = cb->isStatic;

			if (aStatic && bStatic) {
				continue;
			}
			if (aStatic && !bStatic) {
				// b만 움직일 수 있으므로, b를 MTV 반대 방향으로 이동시켜야 함
				tb->position.x -= mtv.x;
				tb->position.y -= mtv.y;

		
			}
			else if (!aStatic && bStatic) {
				// a만 움직일 수 있으므로, a를 MTV 방향으로 이동시켜야 함
				ta->position.x += mtv.x;
				ta->position.y += mtv.y;

	
			}
			else {
				// 둘 다 움직일 수 있다면 반반 나눔
				ta->position.x += mtv.x * 0.5f;
				ta->position.y += mtv.y * 0.5f;
				tb->position.x -= mtv.x * 0.5f;
				tb->position.y -= mtv.y * 0.5f;
			}

			if (bodyA && !ca->isStatic) {
				if (mtv.x != 0 && bodyA->velocity.x * mtv.x < 0)
					bodyA->velocity.x = 0;
				if (mtv.y != 0 && bodyA->velocity.y * mtv.y < 0)
					bodyA->velocity.y = 0;
			}
			if (bodyB && !cb->isStatic) {
				if (mtv.x != 0 && bodyB->velocity.x * mtv.x > 0) // 반대방향!
					bodyB->velocity.x = 0;
				if (mtv.y != 0 && bodyB->velocity.y * mtv.y > 0)
					bodyB->velocity.y = 0;
			}

		}
	}


};