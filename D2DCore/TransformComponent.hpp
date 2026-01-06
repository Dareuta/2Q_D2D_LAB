//========================================================================
// TransformComponent.hpp
//========================================================================

#pragma once

#include <d2d1.h>
#include "IComponent.hpp"

class TransformComponent : public IComponent {
private:
	D2D1_MATRIX_3X2_F worldMatrix = D2D1::Matrix3x2F::Identity(); //단위행렬로 초기화
	bool dirty = true; // 변경이 발생했는지 확인하는 플래그 < 다시 계산해야함

	//부모 포인터를 가지고있음
	TransformComponent* parent = nullptr;	//S,R,T와 다르게, 이건 그렇게 자주 안꺼내봄 + 안정성을 중요시 생각했음												

public: // 너무 자주 사용하는 요소이기 때문에, 캡슐화보다 성능우선으로 public으로 선언 + 이미 변수안에 계산값이 들어가있어서 바로 사용해도 됨
	D2D1_POINT_2F position = { 0.0f, 0.0f }; //위치
	float rotation = 0.0f; // 라디안
	D2D1_SIZE_F scale = { 1.0f, 1.0f }; //스케일	

	void SetParent(TransformComponent* p) {
		parent = p;
		dirty = true; //부모가 바뀌면, 다시 계산해야함
	}

	TransformComponent* GetParent() const { //순회나 탐색할때 쓰는거임
		return parent;
	}

	void SetPosition(float x, float y) { // x, y 받아서 
		if (position.x != x || position.y != y) { //변하지 않았는데 옮길필요 없잖슴
			position = { x, y }; //집어넣고
			dirty = true; // 변경 플래그 온
		}
	}

	void SetRotation(float r) { // 값 받고
		rotation = r; // 적용하고
		dirty = true; // 플래그 키고 - 변경여부를 알림
	}

	void SetScale(float sx, float sy) { // 마찬가지
		scale = { sx, sy };
		dirty = true;
	}

	//const가 뒤에 붙으면 맴버를 수정 안한다는거
	const D2D1_MATRIX_3X2_F& GetWorldMatrix() { 	//지금의 경우, 반환하는 값을 수정못하게 막겠다는거
		if (dirty) { // 어디선가 정상적인 루트로 값이 바뀌었다면
			//곱하는 순서는 S > R > T 이어야함
			D2D1_MATRIX_3X2_F localMatrix =
				D2D1::Matrix3x2F::Scale(scale, { 0.0f, 0.0f }) *
				D2D1::Matrix3x2F::Rotation(rotation, { 0.0f, 0.0f }) *
				D2D1::Matrix3x2F::Translation(position.x, position.y);

			if (parent)//부모가 있다면? 말이좀 이상하네
			{
				//아름다운 구조다. 각 반환형이 const D2D1_MATRIX_3X2_F& 라서, 마치 체인처럼 연결이 된다
				//const << 역류 방지, & << 복사 안함 = 체인
				worldMatrix = localMatrix * parent->GetWorldMatrix(); //부모 행렬이랑 곱해서 반환
			}
			else { //부모가 없다면, 더이상하네
				worldMatrix = localMatrix; // 자기 계산값 반환
			}

			dirty = false; //변경했으니, 플래그 끔
		}

		return worldMatrix; //월드좌표 행렬 반환
	}
};
