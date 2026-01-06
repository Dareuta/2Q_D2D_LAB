//========================================================================
// DemoGameApp.cpp
//========================================================================
/*
#include "DemoGameApp.hpp"
#include "ResourceManager.hpp"


DemoGameApp::DemoGameApp(HINSTANCE hInst) : Application(hInst) {}
//대충, 부모생성자에 넘기기
//다른 클래스가 그 객체를 사용하려면, 그 초기화가 끝난 이후여야만 함.
//즉, hpp에서는 hInst를 넘길 수 없음, 생성되지 않았기 때문임

bool DemoGameApp::Initialize() {
	if (!Application::Initialize()) return false;	
	pImage = ResourceManager::GetInstance().LoadBitmapFromFile(L"assets\\test.png");
	return pImage != nullptr;
}

void DemoGameApp::Render() {
	Direct2D::GetInstance().BeginDraw();
	Direct2D::GetInstance().Clear(D2D1::ColorF(D2D1::ColorF::Black));

	if (pImage) {
		// 윈도우 크기 얻기
		RECT rect;
		GetClientRect(hWnd, &rect);  // Application protected 멤버인 hWnd 사용 가능
		float winWidth = static_cast<float>(rect.right - rect.left);
		float winHeight = static_cast<float>(rect.bottom - rect.top);

		// 이미지 크기 얻기
		D2D1_SIZE_F size = pImage->GetSize();

		// 중앙 좌표 계산
		float x = (winWidth - size.width) / 2.0f;
		float y = (winHeight - size.height) / 2.0f;

		// 이미지 출력
		Direct2D::GetInstance().GetRenderTarget()->DrawBitmap(
			pImage,
			D2D1::RectF(x, y, x + size.width, y + size.height),
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
		);
	}

	Direct2D::GetInstance().EndDraw();
}
*/