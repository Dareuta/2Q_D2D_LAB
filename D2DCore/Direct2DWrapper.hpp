//========================================================================
// Direct2DWrapper.hpp
//========================================================================

#pragma once

#include <d2d1.h>
#include <wincodec.h>
#include <dwrite.h>

#include "Singleton.hpp"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "windowscodecs.lib")

class Direct2D : public Singleton<Direct2D> {
	friend class Singleton<Direct2D>;
private:
	ID2D1Factory* pFactory = nullptr;
	//  Direct2D의 핵심 공장 객체. 렌더타겟 등 여러 객체를 만듦

	ID2D1HwndRenderTarget* pRenderTarget = nullptr;
	// 윈도우용 렌더 타겟. BeginDraw() ~ EndDraw() 사이에서 여기에 그림

	IWICImagingFactory* pWICFactory = nullptr;
	// 이미지 디코딩용 WIC 팩토리. PNG, JPEG 등을 Direct2D 비트맵으로 변환할 때 필요

	IDWriteFactory* dwriteFactory = nullptr;
	// 폰트 공장임
	

public:
	bool Initialize(HWND hWnd);
	void BeginDraw();
	void EndDraw();
	void Clear(D2D1::ColorF colr);

	ID2D1HwndRenderTarget* GetRenderTarget() const { return pRenderTarget; }
	IWICImagingFactory* GetWICFactory() const { return pWICFactory; }
	IDWriteFactory* GetDWriteFactory() const { return dwriteFactory; }

	// ResourceManager에서 처리해서 삭제함
	//ID2D1Bitmap* LoadBitmapFromFile(const wchar_t* filename); // 이미지 파일 로딩
};