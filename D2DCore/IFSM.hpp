//========================================================================
// IFSM.hpp
//========================================================================

#pragma once

#include "pch.h"
#include "GameObject.hpp"
#include <string>

/*
* D2DCore에서는, FSM에 대한 인터페이스를 제공하고 그걸 기반으로
* 각각 FSM을 코딩해줘야함, 즉, 여기서 제공되는 인터페이스를 만족해야
* 내부 구조가 돌아감, 상태는 Wstring으로 이동하며, 하위 FSM 내부에서 변환함
* 
* FSM 추후에 클래스로 묶어서 관리하는걸 해보자, 지금 구조는 너무 뭉텅이라 유지보수가 어려움
*/

enum class InputState { Default, KeyA, KeyB, KeyC, Right, Left, RightUp, LeftUp, Up, Down }; // 굳이 여기 없어도 되긴 함

class IFSM { // 모든 FSM은 해당 클래스를 상속받아서 구현해야함
public:
	virtual ~IFSM() = default;

	virtual void Init() = 0;
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update(float delta, InputState input = InputState::Default) = 0;
	virtual bool TrySetState(const std::wstring& state) = 0;
	virtual void RegistOwner(GameObject* owner) = 0;
};