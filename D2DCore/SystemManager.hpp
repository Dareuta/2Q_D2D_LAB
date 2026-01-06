//========================================================================
// SystemManager.hpp
//========================================================================

#pragma once

#include "InputSystem.hpp"
#include "TimeSystem.hpp"
#include "ResourceManager.hpp"
#include "Direct2DWrapper.hpp"
#include "SceneManager.hpp"
#include "Singleton.hpp"
#include "InputEventSystem.hpp"
#include "TextFormatCache.hpp"
#include "BrushManager.hpp"
#include "AnimationSystem.hpp"

class SystemManager : public Singleton<SystemManager> {
	friend class Singleton<SystemManager>;

public:
	void Initialize(HWND hwnd) {
		Direct2D::GetInstance().Initialize(hwnd);
		InputSystem::GetInstance().Startup(hwnd);
		TimeSystem::GetInstance().StartUp();
		ResourceManager::GetInstance().Initialize(
			Direct2D::GetInstance().GetRenderTarget(),
			Direct2D::GetInstance().GetWICFactory()
		);

		TextFormatCache::GetInstance().Initialize(
			Direct2D::GetInstance().GetDWriteFactory()
		);

		BrushManager::GetInstance().Initialize(
			Direct2D::GetInstance().GetRenderTarget()
		);

		/* 등록 방식을 변경함, 씬을 알고있게 하는게 효율적이지 못하다고 판단 랜더시스템처럼, objects를 넘겨서 처리하는 방식으로 변경함
		SceneManager::GetInstance().BindOnInvokeAnimationSystem( // 씬 전환될때마다, 델리게이트 호출되서 변경된 씬을 전달해주는거임
			[](Scene* scene) {AnimationSystem::GetInstance().SetCurrentAnimationScene(scene);} // 나 이제 람다함수좀 잘 쓰는듯
		);
		*/

	}

	void Update() {
		TimeSystem::GetInstance().Update();
		InputSystem::GetInstance().Update();
		InputEventSystem::GetInstance().Update();
		AnimationSystem::GetInstance().Update();		
	}

	void Shutdown() {
		//ResourceManager::GetInstance().Shutdown(); 비트맵 소유권 이전됨, 더이상 사용 X

		TextFormatCache::GetInstance().Shutdown();
		BrushManager::GetInstance().Shutdown();
		CoUninitialize();
	}
};
