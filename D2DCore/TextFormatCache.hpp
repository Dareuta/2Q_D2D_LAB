//========================================================================
// TextFormatCache.hpp
//========================================================================

#pragma once
#include <unordered_map>
#include <string>
#include <dwrite.h>
#include "Singleton.hpp"

class TextFormatCache : public Singleton<TextFormatCache> {
	friend class Singleton<TextFormatCache>;
private:
	std::unordered_map<std::wstring, IDWriteTextFormat*> textCache;
	IDWriteFactory* factory = nullptr;

public:
	void Initialize(IDWriteFactory* f) { factory = f; }

	// !!!주의!!! IDWriteTextFormat는 폰트 사이즈마다 다른 객체로 생성됨
	// 같은 key에 서로 다른 size로 GetFormat() 호출 시, 캐시 충돌/오동작 발생함
	// 반드시 key에 사이즈 포함해서 사용 (Title_32, Body_24 등)
	IDWriteTextFormat* GetFormat(const std::wstring& key, float size = 24.0f, const std::wstring& font = L"맑은 고딕") { //키만 입력해도 작동함
		if (!factory) return nullptr; // 초기화 안했으면 작동안함!

		auto it = textCache.find(key); //캐쉬 읽어서
		if (it != textCache.end()) return it->second; //있으면 바로 반환

		IDWriteTextFormat* format = nullptr; //릴리즈 필요함!!!

		factory->CreateTextFormat( //펙토리로 텍스트 포멧 만드는거임
			font.c_str(),		//폰트 이름을 C스타일로, 리소스매니저 내부 디코더?에서는 이미지 경로가 들어갔음
			nullptr,			// 이건 또 왜 nullptr이지
			DWRITE_FONT_WEIGHT_NORMAL,	//아무튼
			DWRITE_FONT_STYLE_NORMAL,	//전부
			DWRITE_FONT_STRETCH_NORMAL,	//노말씀 
			size,		// 사이즈는 사이즈임, 그것이 사이즈니까
			L"ko-kr",	//한국어 기본
			&format		// 여기담아주세용
		);

		if (format) textCache[key] = format; // 생성에 성공했다면 맵에 등록(캐싱)
		return format; //바로쓰라고 돌려줌
	}

	void Shutdown() {
		for (auto& pair : textCache) { // 전부 순회하면서
			if (pair.second)
			{
				pair.second->Release(); // 릴!!!!!!리!!!!!즈!!!!!!!
				pair.second = nullptr;	// 명시적 널처리
			}
		}
		textCache.clear(); // 비워
	}
};