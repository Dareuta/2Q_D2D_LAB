//========================================================================
// KinematicBodyComponent.hpp
//========================================================================

#pragma once

#include "IComponent.hpp"
#include "d2d1.h"

struct KinematicBodyComponent : public IComponent {
	D2D1_POINT_2F direction = { 0.0f, 0.0f }; // 정규화된 백터 - 방향백터?
	float speed = 0.0f; // 속력
};