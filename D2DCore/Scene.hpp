//========================================================================
// Scene.hpp
//========================================================================

#pragma once

#include <vector>
#include <memory>
#include "GameObject.hpp"

class Scene {
protected:
	std::vector<std::shared_ptr<GameObject>> objects;

public:
	virtual ~Scene() = default;

	virtual void Initialize() = 0;
	virtual void Update(float delta) = 0;
	virtual void Render() = 0;

	virtual void OnEnter() {}
	virtual void OnExit() {}

	void AddObject(const std::shared_ptr<GameObject>& obj) {
		objects.push_back(obj);
	}

	void RemoveObject(const std::shared_ptr<GameObject>& obj) {
		objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end()); //백터에서만 지워주면 알아서 스마트포인터라 사라짐
	}

	const std::vector<std::shared_ptr<GameObject>>& GetObjects() const {
		return objects;
	}
};