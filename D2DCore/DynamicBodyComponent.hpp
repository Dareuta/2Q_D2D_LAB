//========================================================================
// DynamicBodyComponent.hpp
//========================================================================

#pragma once

#include "IComponent.hpp"
#include <d2d1.h>

struct DynamicBodyComponent : public IComponent {
	D2D1_POINT_2F velocity = { 0.0f, 0.0f }; // X, Y에 대한 속력
	D2D1_POINT_2F acceleration = { 0.0f, 0.0f }; // X, Y에 대한 가속도 (한 프레임에만 적용될 힘임)
	float gravityScale = 1.0f; // 중력 계수
	float mass = 1.0f; // mtv 구할때, 비율적으로 적용 가능함(아직 안함)

	bool useGravity = true; // 중력이 적용되는 중인지 여부, 지상 or 공중

	void AddForce(D2D1_POINT_2F force) { 
		if (mass <= 0.0f) return; // 질량이 없는물체, 그것은 광자일까
		acceleration.x += force.x / mass; //그 유명한 F = MA 아니겠더냐
		acceleration.y += force.y / mass; //식을 살짝 뒤집어서 A = F / M 이겠네
	}

};