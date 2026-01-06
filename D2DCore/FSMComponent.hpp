//========================================================================
// FSMComponent.hpp
//========================================================================

#pragma once

#include "pch.h"
#include <memory>
#include <string>
#include "IComponent.hpp"
#include "IFSM.hpp"

class FSMComponent : public IComponent {
private:
	std::unique_ptr<IFSM> fsmSystem;
	std::wstring currentState;

public:
	InputState inputState; // 그냥 외부 공개해버리고, 캐싱해서 키 입력 처리하면 될듯? 굳이 숨길 이유가 없음

	void RegisteSystem(std::unique_ptr<IFSM> fsm) { // 해당 컴포넌트가 사용할 시스템을 등록해줘야함.
		if (fsm) {
			fsm->RegistOwner(this->owner);
			fsmSystem = std::move(fsm);
		}
	}

	void Enter() {
		// 자식에서 구현하는데, 강제성은 없음
		if (fsmSystem) fsmSystem->Init();
	}

	void Update(float delta) {
		if (fsmSystem)
			fsmSystem->Update(delta, inputState);
	}

	void SetState(const std::wstring& next) {
		if (fsmSystem && currentState != next) {
			if (fsmSystem->TrySetState(next)) {
				currentState = next;
			}
		}
	}
};
