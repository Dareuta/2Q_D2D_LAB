//========================================================================
// ColliderComponent.hpp
//========================================================================

#pragma once

#include "IComponent.hpp"
#include <d2d1.h>

struct ColliderComponent : public IComponent {
	D2D1_SIZE_F size = { 50.0f, 50.0f }; // AABB 영역
	D2D1_POINT_2F offset = { 0.0f, 0.0f }; // Transform 기준 오프셋
	bool isTrigger = false; // 트리거(충돌 여부)
	bool isStatic = false;

	// AABB 계산, 트랜스폼 기준으로 박스 생성함
	D2D1_RECT_F GetAABB(const D2D1_POINT_2F& worldPos) const {
		float halfW = size.width * 0.5f;
		float halfH = size.height * 0.5f;
		float cx = worldPos.x + offset.x;		
		float cy = worldPos.y + offset.y;

		return D2D1::RectF(cx - halfW, cy - halfH, cx + halfW, cy + halfH);
	}
};