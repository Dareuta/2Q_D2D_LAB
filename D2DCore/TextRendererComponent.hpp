//========================================================================
// TextRendererComponent.hpp
//========================================================================

#pragma once

#include <string>
#include <d2d1.h>

#include "IComponent.hpp"
#include "BitmapResource.hpp"

class TextRendererComponent : public IComponent {
public:
	std::wstring text = L"";
	std::wstring font = L"맑은 고딕";
	std::wstring formatKeyOverride = L""; // (옵션) 외부에서 강제 포맷 키 설정
	D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::White);
	D2D1_SIZE_F layoutSize = { 2000.0f, 200.0f }; // 레이아웃(패널크기)
	float fontSize = 28.0f;
	bool screenSpace = true; // 화면고정 여부
};