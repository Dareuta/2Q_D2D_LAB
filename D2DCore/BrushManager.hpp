//========================================================================
// BrushManager.hpp
//========================================================================

#pragma once
#include "Singleton.hpp"
#include "d2d1.h"

class BrushManager : public Singleton<BrushManager> { //브러쉬 색 바꿔서 쓸려고 만든 매니저임, 필요한 색 만들어두고 이곳저곳에서 불러씀
	friend class Singleton<BrushManager>;
private:
	ID2D1SolidColorBrush* whiteBrush = nullptr;

public:
	void Initialize(ID2D1RenderTarget* rt) {
		rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush); //흰 브러쉬 만듬
	}

	ID2D1SolidColorBrush* GetWhiteBrush() const { return whiteBrush; } //색깔별로 만들어둘 수 있음

	void Shutdown() {
		if (whiteBrush) whiteBrush->Release(); // 있으면 릴리즈 이것도 색깔별로 해줘야함
		whiteBrush = nullptr; //대충 관습적인 널처리
	}
};