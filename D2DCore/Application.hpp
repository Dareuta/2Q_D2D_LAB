//========================================================================
// Application.hpp
//========================================================================

#pragma once
#include <windows.h>

class Application {
protected:
	HWND hWnd; // 만들어진 윈도우의 핸들, 메세지 처리 / 그리기에 사용됨 // 윈도우를 구분하는 ID
	//메세지를 받을때 사용한다고 함

	HINSTANCE hInstance; // 현재 실행중인 인스턴스 핸들 << 윈도우 클래스 등록에 필요
	//반대로, OS가 구분하기 위해 사용한다고 함

public:
	Application(HINSTANCE hInst); //생성자에서 HINSTANCE를 객체 내부에 저장함
	virtual ~Application() = default;

	virtual bool Initialize();
	virtual int Run(); // 메세지 루프 돌리는 역할

protected:
	virtual void Update(float delta);
	virtual void Render();

	//실제 메세지 처리용 함수
	//WM_KEYDOWN, WM_PAINT 같은 메세지들이 이 함수로 처리됨
	//자식클래스에서 커스텀 메세지 처리를 할 수 있게 해줌
	virtual LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//OS가 호출하는 전역 콜백함수
	//이거는 꼭 있어야함
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};