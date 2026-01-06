//========================================================================
// AnimationComponent.hpp
//========================================================================

#pragma once

#include <unordered_map>
#include <memory>
#include <functional>

#include "IComponent.hpp"
#include "AnimationClip.hpp"
#include "FrameData.hpp"

struct AnimationClipComponent : public IComponent { // 정적 정보들
	std::unordered_map<std::wstring, std::shared_ptr<AnimationClip>> clips; // 해당 컴포넌트가 붙은 오브젝트가 사용할 모든 애니메이션이 담기는 맵임
	// key는 가능하면 클립 이름과 일치시키는게 관리하기 편할듯?															
	std::shared_ptr<SheetData> sheetData; // 이건 시트임, 여기있는 정보를 위에 있는 해싱된 클립들과 조합해서 애니메이션을 재생시킴
};

struct AnimationStateComponent : public IComponent { // 동적 정보(현재 상태)
private:
	std::wstring currentClip; // 지금 클립 - 해싱된 수많은 클립중 어떤걸 재생중인지
public:
	int currentFrame = 0; // 지금 프레임
	float timer = 0.0f; // 애니메이션프레임 구조체가 가지고있는 듀레이션(시간)을 관리하기 위해서 지금 얼마나 시간이 흘렀는지 측정해야함\

	bool isFinished = false;
	std::function<void()> onClipFinished = nullptr;

	void SetClip(const std::wstring& newClip) {
		if (currentClip != newClip) {
			currentClip = newClip;
			currentFrame = 0;
			timer = 0.0f;
			isFinished = false;			
		}
	}

	const std::wstring& GetClip() const { return currentClip; }
};