//========================================================================
// CameraSystem.hpp
//========================================================================

#pragma once

#include <d2d1.h>

class CameraSystem {
private:
	D2D1_POINT_2F position = { 0.0f, 0.0f }; // 월드좌표에서 카메라의 위치
	float rotation = 0.0f;					 // 회전(라디안)
	float zoom = 1.0f;						 // 줌 << 기본값 1.0

public:

	void MoveBy(float dx, float dy) { //이동
		position.x += dx;
		position.y += dy;
	}
	void SetPosition(float x, float y) { position = { x, y }; }

	void AddRotation(float dr) { rotation += dr; }
	void SetRotation(float r) { rotation = r; }

	void AddZoom(float dz) {
		if (dz + zoom > 10.0f) zoom = 10.0f;
		else if (dz + zoom < 0.1f) zoom = 0.1f;
		else zoom += dz;
	}

	void SetZoom(float z) {
		if (z > 10.0f) z = 10.0f; // 최대 값
		else if (z < 0.1f) z = 0.1f; // 최소 값

		zoom = z;
	}

	D2D1_POINT_2F GetPosition() const { return position; }
	float GetRotation() const { return rotation; }
	float GetZoom() const { return zoom; }

	D2D1_MATRIX_3X2_F GetViewMatrix(float screenWidth, float screenHeight) const {

		D2D1_POINT_2F center = { screenWidth / 2.0f,screenHeight / 2.0f };

		auto S = D2D1::Matrix3x2F::Scale(zoom, zoom, center); // center를 기준으로 키운다는 뜻
		auto R = D2D1::Matrix3x2F::Rotation(-rotation, center); // -붙여서 반대로 돌림
		auto T = D2D1::Matrix3x2F::Translation(-position.x, -position.y); // 좌표도 반대로

		//zoom을 제외하고는, 반대로 곱해줘야함

		return S * R * T; //기차이름이 SRT던가 아무튼 순서 중요함
	}
};
