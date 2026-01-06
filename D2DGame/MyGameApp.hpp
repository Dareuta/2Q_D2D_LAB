//========================================================================
// MyGameApp.hpp
//========================================================================

#pragma once

#include "Application.hpp"

#define SCREEN_WIDTH 1920 // application 내부에 하드코딩되어있음, 나중에 찾아서 바꿔야함 수정하거나
#define SCREEN_HEIGHT 1080

class MyGameApp : public Application {
private:
	
public:
	MyGameApp(HINSTANCE hInst);
	virtual ~MyGameApp();

	virtual bool Initialize() override;        // 초기화
	virtual void Update(float delta) override; // 게임 로직 처리
	virtual void Render() override;            // 그리기
};