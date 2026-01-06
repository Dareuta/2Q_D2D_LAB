//========================================================================
// CollisionFlagComponent.hpp
//========================================================================

#pragma once

#include "IComponent.hpp"
#include "GameObject.hpp"
#include "Delegate.hpp"
#include <unordered_set>

struct CollisionFlagComponent : public IComponent {
    std::unordered_set<GameObject*> currentHits;   // 현재 프레임 
    std::unordered_set<GameObject*> previousHits;  // 이전 프레임

    // 델리게이트 외부에서 함수 등록해두면, 시스템에서 판단하고 호출해줌
    MultiDelegate<GameObject*> onEnter;
    MultiDelegate<GameObject*> onStay;
    MultiDelegate<GameObject*> onExit;

    // 현재상태를 이전상태로 밀어버림, 초기화 + 저장
    void Sync() {
        previousHits = std::move(currentHits);
        currentHits.clear();
    }
};