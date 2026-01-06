//========================================================================
// SpriteRendererComponent.hpp
//========================================================================

#pragma once

#include <d2d1.h>
#include <functional>
#include <memory>

#include "BitmapResource.hpp"
#include "IComponent.hpp"

// shared_ptr의 소유권을 컴포넌트로 옮김

class SpriteRendererComponent : public IComponent { //ECS구조 특성상, 컴포넌트는 데이터를 담기만함
private:
	// ID2D1Bitmap* bitmap = nullptr; // 이미지 정보 // 기존 로우포인터에서, 쉐어드ptr로 변경함
	std::shared_ptr<BitmapResource> bitmapResource = nullptr;

	D2D1_RECT_F sourceRect = { 0, 0, 0, 0 }; // 부분 랜더링 크기

	int zOrder = 0; // 우선순위 비교(낮으면 뒤로감)
public:
	D2D1_POINT_2F pivot = { 0.5f, 0.5f }; // 기본값 0.5 0.5 << 정규화된 비율임

	std::function<void()> onZOrderChanged; // 델리게이트 << Z갑 변경되는걸 알려주는 역할

	void SetBitmapResource(const std::shared_ptr<BitmapResource>& re) {
		bitmapResource = re;  // 내부에 저장해두고
		if (re && re->Get()) { // 전부 있고, 비트맵도 있으면
			auto size = re->Get()->GetSize(); // -> -> 오... 아무튼 비트맵 내부에서 사이즈 받아오는거임
			sourceRect = { 0,0,size.width, size.height };
		}
	}

	/* // 로우포인터 사용하는 방식 수정해서 주석처리함
	void SetBitmap(ID2D1Bitmap* bmp) {
		bitmap = bmp;
		if (bmp) {
			auto size = bmp->GetSize(); //부분랜더링 크기를, 원본 이미지로 설정하는것
			sourceRect = { 0, 0, size.width, size.height };
		}
	}
	ID2D1Bitmap* GetBitmap() const { return bitmap; }
	*/

	ID2D1Bitmap* GetBitmap() const {
		if (bitmapResource) // shared_ptr 등록되어있으면, 내부 로우포인터(bitmap) 꺼내서 건내줌
			return bitmapResource->Get();
		else
			return nullptr;
	}

	// 부분 랜더링
	void SetSourceRect(const D2D1_RECT_F& rect) { sourceRect = rect; }
	const D2D1_RECT_F& GetSourceRect() const { return sourceRect; }

	void SetZOrder(int z) {
		if (zOrder != z) { //새로 입력받은 값이 같으면 처리 안해도 됨
			zOrder = z; //변경
			if (onZOrderChanged) onZOrderChanged(); //호출하는거임 순서 정렬하라고
		}
	}

	int GetZOrder() const { return zOrder; }

};