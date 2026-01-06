//========================================================================
// BitmapResource.hpp
//========================================================================

#pragma once

#include "pch.h"

#include <d2d1.h>

// com 객체(ID2D1Bitmap)는 릴리즈를 꼭 해줘야하는데,
// 기본 스마트 포인터들은 릴리즈를 안해주고 혼자 사라져버리는게 문제라서
// 이 객체를 통해 릴리즈를 보장함, 즉 RAII를 사용하기 위한 껍데기임

class BitmapResource {
private:
	ID2D1Bitmap* bitmap = nullptr;

public:
	BitmapResource(ID2D1Bitmap* bmp) : bitmap(bmp) {}
	~BitmapResource() {
		if (bitmap) {
			bitmap->Release();
			bitmap = nullptr;
		}
	} // 소멸자에서 릴리즈 보장

	BitmapResource(const BitmapResource&) = delete; //rule of five 아무튼 다 금지함 ㅅㄱ
	BitmapResource& operator=(const BitmapResource&) = delete;

	BitmapResource(BitmapResource&&) = delete;
	BitmapResource& operator=(BitmapResource&&) = delete;

	ID2D1Bitmap* Get() const { return bitmap; } //어차피, bitmap 관리하기 위한 클래스라서, 간단하게 적어도 됨
};