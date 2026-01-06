//========================================================================
// InputEventSystem.hpp
//========================================================================

#pragma once

#include "pch.h"

#include "Delegate.hpp"
#include "InputSystem.hpp"
#include "Singleton.hpp"

class InputEventSystem : public Singleton<InputEventSystem> {
	friend class Singleton<InputEventSystem>;
private:
	std::array<SHORT, 256> prevState = { 0 };

public:
	MultiDelegate<int> onKeyPressed;
	MultiDelegate<int> onKeyReleased; // !!!이거 지금, 유효성 검사가 여러개면 문제가 생김 나중에 사용할때 수정해야함!!!

	void Update() {
		for (int i = 0; i < 256; ++i) {
			bool wasDown = (prevState[i] & 0x8000) != 0; // 0x8000 = 1000 0000 0000 0000 // 비트연산
			bool isDown = (GetAsyncKeyState(i) & 0x8000) != 0;


			if (!wasDown && isDown)
				onKeyPressed.Broadcast(i); // 해당 키를 구독한 대상에게 전부 신호를 줌

			if (wasDown && !isDown)
				onKeyReleased.Broadcast(i);

			prevState[i] = static_cast<SHORT>(GetAsyncKeyState(i)); //비교 이후, 현재 상태를 이전상태에 저장
		}
	}
};
