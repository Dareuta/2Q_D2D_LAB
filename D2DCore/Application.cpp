//========================================================================
// Application.cpp
//========================================================================

#include "pch.h"

#include "Application.hpp"
#include "SystemManager.hpp"

Application::Application(HINSTANCE hInst) : hInstance(hInst), hWnd(nullptr) {}
//이건 헤더에 놔도 괜찮지만, 성능적으로 cpp에 놓는게 좋다고함

bool Application::Initialize() {
	// [1] 윈도우 클래스(WNDCLASSEX) 구조체 초기화
	// 이 구조체는 윈도우를 만들기 위한 "템플릿"처럼 사용됨
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);                        // 구조체 크기
	wc.style = CS_HREDRAW | CS_VREDRAW;                    // 창 크기 변경 시 WM_PAINT 발생
	wc.lpfnWndProc = Application::WndProc;                 // 메시지 처리 함수 (정적 함수여야 함)
	wc.hInstance = hInstance;                              // 현재 실행 중인 프로그램 인스턴스 핸들
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);           // 기본 커서 설정 (화살표)
	wc.lpszClassName = L"MyWindowClass";                   // 윈도우 클래스 이름 (이름으로 식별됨)

	// [2] 윈도우 클래스 등록
	// OS에 "이런 이름의 클래스가 있어요" 라고 알려주는 단계
	if (!RegisterClassEx(&wc))
		return false;

	// + 화면 크기 정확하게 맞춰주는 부분
	RECT rc = { 0, 0, 1920, 1080 }; // 하드코딩해둠, 나중에 바꿔야함
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	int windowWidth = rc.right - rc.left;
	int windowHeight = rc.bottom - rc.top;

	// [3] 윈도우 생성
	// 실제 윈도우 객체를 생성. 이 함수가 성공하면 내부적으로 HWND 핸들이 생성됨
	hWnd = CreateWindowEx(
		0,                           // 확장 스타일 (보통 0)
		L"MyWindowClass",            // 등록한 클래스 이름
		L"DemoGameApp",              // 윈도우 타이틀 (창 제목 표시줄)
		WS_OVERLAPPEDWINDOW,         // 일반적인 윈도우 스타일 (닫기/최소화/최대화 가능)
		CW_USEDEFAULT, CW_USEDEFAULT,// 위치 자동 결정
		windowWidth, windowHeight, // 창 크기 (가로, 세로)
		nullptr, nullptr,            // 부모 윈도우, 메뉴 핸들 (없음)
		hInstance,                   // 애플리케이션 인스턴스
		this                         // 마지막 매개변수는 WM_NCCREATE에서 받아 this 저장용
	);

	// [4] 윈도우 생성 실패 시 실패 반환
	if (!hWnd)
		return false;

	// [5] 생성한 윈도우를 화면에 보이게 함
	ShowWindow(hWnd, SW_SHOW);  // SW_SHOW: 바로 보여줌
	UpdateWindow(hWnd);         // WM_PAINT 강제로 보내서 초기 그리기 유도

	// [6] 전역 시스템 초기화 
	SystemManager::Create();
	SceneManager::Create();
	TimeSystem::Create();
	InputSystem::Create();
	ResourceManager::Create();
	Direct2D::Create();
	InputEventSystem::Create();
	TextFormatCache::Create();
	BrushManager::Create();
	AnimationSystem::Create();


	SystemManager::GetInstance().Initialize(hWnd);

	return true; // 성공적으로 초기화 완료
}

int Application::Run() {
	MSG msg = {};

	// 루프 계속 돌면서 메시지 있는 경우만 처리
	while (true) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { //GetMessage()여서 안되는거였음, 수정완료
			if (msg.message == WM_QUIT) { //시스템 종료 호출
				SystemManager::GetInstance().Shutdown();
				return static_cast<int>(msg.wParam); 
			}				

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 매 프레임 호출 // 프레임보다 빠르게 호출인듯 그냥 계~속 호출함
		SystemManager::GetInstance().Update();  // 여기서 TimeSystem, InputSystem 갱신됨

		float delta = TimeSystem::GetInstance().GetDelta();

		Update(delta);   // 게임 로직 // 델타 넘겨줌
		Render();   // 렌더링
	}
}


void Application::Update(float delta) {
	// 기본 비어있음
}

void Application::Render() {
	// 기본 비어있음
}

//======================================================================================
// 메시지를 클래스 내부에서 처리하는 가상 함수
// 실제 메시지 처리는 이 함수에서 하며, 자식 클래스에서 오버라이딩 가능
//======================================================================================
LRESULT Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY:
		// 사용자가 창을 닫았을 때 발생하는 메시지
		// 이 메시지를 처리하면서 애플리케이션을 종료하도록 요청함
		PostQuitMessage(0);  // 메시지 루프 종료 트리거. Run() 루프에서 종료됨.
		return 0;            // 이 메시지를 직접 처리했으므로 0을 반환

		// 여기에 WM_KEYDOWN, WM_PAINT 등 추가할 수 있음 (자식 클래스에서 확장 가능)
	}

	// 처리하지 않은 메시지는 기본 처리기로 넘김
	// ex: 마우스 이동, 창 크기 조절 등 운영체제가 알아서 처리하게 위임
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//======================================================================================
// Win32에서 호출하는 전역 메시지 콜백 함수
// 이 함수는 반드시 static이거나 전역이어야 하며, 클래스 인스턴스 접근 불가
// 여기서 내부적으로 this 포인터를 저장한 뒤, 실제 처리함수로 위임한다
//======================================================================================
LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Application* pThis = nullptr;  // 실제 메시지를 처리할 객체 포인터

	if (message == WM_NCCREATE) {
		// CreateWindowEx() 호출 시 전달한 lpParam → 여기서 this 포인터 받음
		// 이 메시지는 윈도우 창이 생성될 때 가장 먼저 오는 메시지 중 하나
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pThis = reinterpret_cast<Application*>(pCreate->lpCreateParams);

		// 이 this 포인터를 HWND에 연결해 저장해 둠
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else {
		// 이미 저장된 this 포인터를 HWND에서 꺼내옴
		// 이후 모든 메시지에서 이걸 통해 인스턴스 접근 가능
		pThis = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pThis) {
		// this 포인터가 있다면 → 해당 객체의 WindowProc으로 위임
		return pThis->WindowProc(hWnd, message, wParam, lParam);
	}

	// 아직 this 포인터를 못 얻은 경우 or 예외 상황 → 기본 처리
	return DefWindowProc(hWnd, message, wParam, lParam);
}
