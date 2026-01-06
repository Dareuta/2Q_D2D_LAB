//========================================================================
// MyGameApp.cpp
//========================================================================

#include "MyGameApp.hpp"
#include "SceneManager.hpp"
#include "TitleScene.hpp"
#include "VRAMTestScene.hpp"
#include "GameScene.hpp"
#include "AnimationScene.hpp"
#include "FSMScene.hpp"
#include "PhysicsScene.hpp"

MyGameApp::MyGameApp(HINSTANCE hInst) : Application(hInst) {}

MyGameApp::~MyGameApp() = default;


bool MyGameApp::Initialize() {
	if (!Application::Initialize()) return false; //어플리케이션 초기화 호출 + 검사

	auto& smg = SceneManager::GetInstance();

	smg.RegisterScene<TitleScene>("Title");
	smg.RegisterScene<GameScene>("Game");
	smg.RegisterScene<VRAMScene>("VRAM");
	smg.RegisterScene<AnimationScene>("Animation");	
	smg.RegisterScene<FSMScene>("FSM");
	smg.RegisterScene<PhysicsScene>("Physics");

	smg.BindOnEnter([](Scene* s) {}); // 모든 씬 진입시 공통작업
	smg.BindOnExit([](Scene* s) {}); // 모든 씬 이탈시 공통작업

	smg.SetScene("Title");

	return true;
}

void MyGameApp::Update(float delta) {
	SceneManager::GetInstance().Update(delta);
}

void MyGameApp::Render() {
	SceneManager::GetInstance().Render();
}