//========================================================================
// SceneManager.hpp
//========================================================================

#pragma once

#include <memory>
#include <functional>

#include "Scene.hpp"
//#include "RenderSystem.hpp"
#include "Singleton.hpp"

class SceneManager : public Singleton<SceneManager> {
	friend class Singleton<SceneManager>;
private:
	std::unordered_map<std::string, std::unique_ptr<Scene>> scenePool; // 유니크 포인터라서, 굳이 해제 안해도 됨(편함)
	Scene* currentScene = nullptr;

	std::function<void(Scene*)> onSceneEnter; // 이거는	
	std::function<void(Scene*)> onSceneExit; // 밖에서 등록하는 관심사임, 델리만쥬같은거임

	//std::function<void(Scene*)> invokeAnimationSystem; // 이건, 씬 바뀌는거 알려줄려고 만듬

public:
	template<typename T> // RegisterScene<GameScene> name 이런식으로 쓰겠지
	void RegisterScene(const std::string& name) { //씬 등록하는거, 
		scenePool[name] = std::make_unique<T>(); //언오더맵에 맵핑함
		scenePool[name]->Initialize(); // 등록한 최초 1회만 실행함
	}

	/* 이제 안씀
	template <typename T, typename... Args> //일단 넣어봤음 나중에 봐야함
	void SetSceneT(Args... args) {
		SetScene(std::unique_ptr<Scene>(new T(std::forward<Args>(args)...)));
	}
	*/

	void SetScene(const std::string& name) { //기존에 등록했던 이름 << 가능하면 같은이름으로 하는게 좋을듯?
		auto it = scenePool.find(name);
		if (it == scenePool.end()) return; // 없으면, 리턴

		if (currentScene && onSceneExit) onSceneExit(currentScene);
		if (currentScene) currentScene->OnExit();

		currentScene = it->second.get(); // 로우 포인터 가져와서 반환 		

		if (currentScene) {
			currentScene->OnEnter(); // 이건 항상임, 이니셜라이즈랑 합치는거도 생각해보면 될듯
			if (onSceneEnter) onSceneEnter(currentScene);
			//if (invokeAnimationSystem) invokeAnimationSystem(currentScene);
		}
	}

	void Update(float delta) { if (currentScene) currentScene->Update(delta); }
	void Render() { if (currentScene) currentScene->Render(); }

	void BindOnEnter(std::function<void(Scene*)> f) { onSceneEnter = f; }
	void BindOnExit(std::function<void(Scene*)> f) { onSceneExit = f; }
	//void BindOnInvokeAnimationSystem(std::function<void(Scene*)> f) { invokeAnimationSystem = f; } // 이거 애니메이션 시스템에서 쓸꺼임

	Scene* GetCurrentScene() const { // 유효성 검사에서 사용함
		return currentScene;
	}
};