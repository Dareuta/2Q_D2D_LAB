//========================================================================
// IComponent.hpp
//========================================================================

#pragma once

#include "pch.h" //예외적으로 헤더에 pch.h를 박음

class GameObject;

class IComponent { //모든 컴포넌트는 IComponent를 상속해야함
protected:
	GameObject* owner = nullptr;
public:
	virtual ~IComponent() = default;
	virtual void SetOwner(GameObject* o) { owner = o; }
	GameObject* GetOwner() const { return owner; }
};