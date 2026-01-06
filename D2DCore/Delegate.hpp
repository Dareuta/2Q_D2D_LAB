//========================================================================
// Delegate.hpp
//========================================================================

#pragma once

#include "pch.h"

#include <vector>
#include <functional>
#include <algorithm>

// 유효성 검사는 외부에서 꼭 등록해줘야함
// 사실 안해줘도 잘 되는듯
// 델리만쥬임

template<typename... Args>
class MultiDelegate {
private:
	struct Slot {
		void* instance; //인스턴스에
		std::function<void(Args...)> func; //등록할 함수
	};
	std::vector<Slot> slots;

	std::function<bool(void*)> isValid = nullptr; //유효성 검사는 밖에서 알아서 "해줘" << void*를 사용해서 주체가 뭔지 모름
public:
	void Add(void* instance, const std::function<void(Args...)>& f) {
		slots.push_back({ instance, f });
	}

	void Remove(void* instance) {
		slots.erase( //범위 삭제인데
			std::remove_if(slots.begin(), slots.end(), //처음부터 끝까지를 순회하면서
				[instance](const Slot& s) { return s.instance == instance; }) //여기까지가 자를 시작선 // 한칸씩 순회하면서, true false 반환함 << 이거로 뒤로 옮기는거
			, slots.end()); //여기가 끝나는부분
	}

	void Clear() { slots.clear(); }

	void SetValidation(const std::function<bool(void*)>& validator) { //외부에서 등록한 함수를 등록하는거임, 즉. true false 반환하는부분을 밖에서 정하는거
		isValid = validator;
	} // 이거 등록할때 [~](~) -> bool {~} 이런 형태로 해주면 좋음

	void Broadcast(Args... args) const { //방송(가지고 있는거 전부 호출함)
		for (const auto& s : slots) {
			if (!isValid || isValid(s.instance)) { // 유효성 검사가 없을경우 전부 호출함
				s.func(args...); //이거 호출하기전에, 유효한지 확인해야함.
			}
		}
	}

	void InvokeFor(void* instance, Args... args) { // 특정 인스턴스만 호출할경우
		for (const auto& s : slots) {
			if (s.instance == instance) {
				s.func(args...);
			}
		}
	}

};