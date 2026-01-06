//========================================================================
// InputSystem.hpp
//========================================================================

#pragma once

#include <Windows.h>
#include <cstdint>
#include <array>

#include "Singleton.hpp"

class InputSystem : public Singleton<InputSystem> {
	friend class Singleton<InputSystem>;
private:
	HWND m_hWnd = nullptr;
	POINT m_mouseClient = { 0, 0 };
	std::array<SHORT, 256> m_prevState = { 0 };
	std::array<SHORT, 256> m_currState = { 0 };

public:
	void Startup(HWND hWnd) { m_hWnd = hWnd; }

	void Update() {
		// 마우스 위치 갱신
		::GetCursorPos(&m_mouseClient);
		::ScreenToClient(m_hWnd, &m_mouseClient);

		// 이전 프레임 상태 저장
		m_prevState = m_currState;

		// 현재 키 상태 갱신
		for (int i = 0; i < 256; ++i) {
			m_currState[i] = ::GetAsyncKeyState(i);
		}
	}

	bool IsKeyDown(int vKey) const { return (m_currState[vKey] & 0x8000) != 0; }
	bool IsKeyPressed(int vKey) const { return !(m_prevState[vKey] & 0x8000) && (m_currState[vKey] & 0x8000); }
	bool IsKeyReleased(int vKey) const { return (m_prevState[vKey] & 0x8000) && !(m_currState[vKey] & 0x8000); }
	POINT GetMouseClient() const { return m_mouseClient; }
};