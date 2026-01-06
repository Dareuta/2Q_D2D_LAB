//========================================================================
// GameObject.hpp
//========================================================================

#pragma once

#include <unordered_map>
#include <typeindex> // typeid(T)로 얻은 타입에 정보를, 비교 + hash가능하게 해줌. 즉, 감싸서 키로 만드는거임
#include <memory>
#include <string>

#include "IComponent.hpp"

class GameObject {
private:
	std::unordered_map<std::type_index, std::unique_ptr<IComponent>> components; // 동일 타입을 하나씩 가질 수 있게 만든 맵

public:
	std::string name;
	bool enabled = true; // 랜더링 여부

	//하나만 붙이는 목적임
	template<typename T, typename... Args> // T : 컴포넌트의 타입
	T* AddComponent(Args&&... args) { //게임오브젝트에 붙이는거임 // 템플릿 + &&는 전달참조임, 의미가 달라짐. 무조건 Rvalue 참조 아님

		auto comp = std::make_unique<T>(std::forward<Args>(args)...); // foraward > 성질을 유지하기 위해 사용. &&쓴거랑 같은 이치 << 기존의 속성을 유지하게 만들어줌, 잘못 해석하는일을 방지함
		T* ptr = comp.get(); // get 쓰면, unique_ptr의 raw 포인터를 얻을 수 있음. // 소유권은 여전히 unique_ptr이 가짐

		ptr->SetOwner(this); // 컴포넌트는 자신이 어떤 오브젝트에 달려있는지에 대해 알고있음

		components[typeid(T)] = std::move(comp); // move > 더이상 사용 안할꺼니, 이동해서 가져가게함
		return ptr;
	}

	template<typename T>
	T* GetComponent() {
		auto it = components.find(typeid(T)); // 타입으로 키 찾음
		if (it != components.end()) //있으면
			return static_cast<T*>(it->second.get());//raw포인터 반환

		return nullptr; //없으면 죽어
	}
};