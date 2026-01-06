//========================================================================
// AnimationSystem.hpp
//========================================================================

#pragma once

#include "Singleton.hpp" // 싱글톤 해도 될듯

#include "FrameData.hpp"
#include "AnimationClip.hpp"
#include "AnimationComponent.hpp"
#include "SpriteRendererComponent.hpp"
#include "GameObject.hpp"
//#include "Scene.hpp"
#include "TimeSystem.hpp"

// AnimationState + AnimationClip + SheetData 정보를 읽어서, SpriteRendererComponent에 적용하는 역할임

// 이 시스템이 처리해줘야 할 일
// 가지고 있는 데이터를 기반으로, 스프라이트 랜더러에 다른 이미지를 출력해주는거임
// 모든 오브젝트들을 순환하면서, 애니메이션 컴포넌트를 가지고 있는 오브젝트를 대상으로
// 가지고 있는 데이터와 비교해서, 이미지를 돌려주면 됨
// 스프라이트 랜더러 컴포넌트를 조종함, 소유하는건 아님
// 사실상 고기 뒤집는 집개같은 역할

class AnimationSystem : public Singleton<AnimationSystem> {
	friend class Singleton<AnimationSystem>;
private:
	//Scene* currentScene = nullptr; // 씬매니저에서 공유해주는거임, 이거로 뭐 하는게 아니라 읽는데만 써야함(중요)
	//씬 대신 오브젝트 받는거로 변경함

	std::vector<std::shared_ptr<GameObject>> objects;

public:
	//void SetCurrentAnimationScene(Scene* newScene) { currentScene = newScene; } // 이거 씬매니저에 바인드 되어있음 묶였있는거임
	void SetObjects(const std::vector<std::shared_ptr<GameObject>>& ob) {
		objects.clear(); // 기존에 가지고있던거 비우고
		for (auto& in : ob) {
			RegisterObject(in);
		}
	}

	void RegisterObject(const std::shared_ptr<GameObject>& in) {
		auto state = in->GetComponent<AnimationStateComponent>();
		auto clip = in->GetComponent<AnimationClipComponent>();
		auto render = in->GetComponent<SpriteRendererComponent>();
		if (!state || !clip || !render) return;
		objects.push_back(in);
	}

	void Update() { // 순회해서, 애니메이션 컴포넌트가 달려있는것들 찾아서 갱신해주는거임		

		for (const auto& obj : objects) {
			// currentScene 내부에 있는 오브젝트 배열을 가져옴

			//최초 등록할때, 컴포넌트가 붙어있는거만 남김			
			// 			
			auto stateComp = obj->GetComponent<AnimationStateComponent>();
			auto clipComp = obj->GetComponent<AnimationClipComponent>();
			auto renderComp = obj->GetComponent<SpriteRendererComponent>();

			//if (!stateComp || !clipComp || !renderComp) continue; // 컴포넌트 3개 다 달려있는 오브젝트 찾음, 하나라도 없으면 건너뜀

			//========================================================================
			// 애니메이션 정적 <-> 동적 연동 ///////////////////////////////////////////
			//========================================================================
			// 컴포넌트 3개다 달고있는 오브젝트에 한해서
			// 구조체 안에 있을거 다 있는지 검증하는 부분

			auto itClip = clipComp->clips.find(stateComp->GetClip()); // 현재 재생되고 있는 프레임이 속한 클립 찾는거임
			if (itClip == clipComp->clips.end()) continue; // 없으면 돌아가

			auto& clip = itClip->second; // 현재 프레임이 속한 클립을 꺼내서
			if (clip->frames.empty()) continue; // 비어있으면 돌아가

			//========================================================================
			// 이제부터가 진짜임

			stateComp->timer += TimeSystem::GetInstance().GetDelta(); // 시간누적, 각 프레임별로, 해당 시간만큼 지속해주는 역할임, 프레임별로 타이머가 초기화됨

			// frames는 백터임
			// 즉 여기서 비교하는건, 해당 커렌트 프레임의 듀레이션만큼만 지속한다는 의미임
			// 프레임 드랍 방지 구조인데, 특이함, 랙걸려서 건너뛰는 프레임을 계산해서 처리해줌
			while (stateComp->timer >= clip->frames[stateComp->currentFrame].duration) { // 일단, 타이머(누적)해서 쌓는곳에 충분한 시간 프레임이 유지되면
				stateComp->timer -= clip->frames[stateComp->currentFrame].duration; //그런데 만약, 델타 누적이 이상하게 되면? 여기서 최대치만큼 정리해서 프레임을 찾아줌
				stateComp->currentFrame++; // 백터 인덱스임, 다음 프레임으로 넘겨줌
				// 그러니까, 타이머에 duration을 뺀 결과가, 다음 duration을 빼고도 남는다면 문제가 되는데, 그걸 while로 스킵해주는거임
				// 이게 없으면, 애니메이션이 느리게 보일꺼임, 매번 호출될때마다 조건이 충족되지만, 호출되는 순간순간이
				// 내가 원하는 타이밍보다 느리다보니, 전체적인 프레임이 느려지는거임, 그걸 스킵해주는거임

				if (stateComp->currentFrame >= static_cast<int>(clip->frames.size())) { // 백터 마지막 인댁스(프레임)에 도달하면
					if (clip->looping) { //루프면 처음으로 돌려줘야지
						stateComp->currentFrame = 0;
					}
					else { // 루프가 아니면 끝나야겠지?
						stateComp->currentFrame = static_cast<int>(clip->frames.size()) - 1;

						if (!stateComp->isFinished) { //1회만 처리되는 구조임
							stateComp->isFinished = true;

							if (stateComp->onClipFinished)
								stateComp->onClipFinished();
						}

						break; // 앞에서 증가한 프레임을 뺴주는거임, 백터 크기보다 큰곳에 접근하는걸 방지하고, 마지막 프레임을 유지해주는거임
					}
				}
			}

			//사실상 문제없다면, while은 한번만 통과하는게 정석임

			//========================================================================
			// FrameData 기반으로 아틀라스 변경 ////////////////////////////////////////
			//========================================================================
			// 마찬가지로, 검증하는 부분임

			// 위에서 현재 프레임에 대한 정보 + 애니메이션 정보를 관리한다면, 이 아래부터는 실제 이미지를 관리함

			const auto& frameName = clip->frames[stateComp->currentFrame].spriteName; // 지금 몇번째 조각인지 1대1 맵핑된 이름(키) 꺼내는거임
			const auto& sheet = clipComp->sheetData; // 아틀라스 + json 구조체임, 컴포넌트 내부에서 애니메이션 정보랑 같이 관리됨
			if (!sheet) continue; // 시트 없으면 나가

			auto itFrame = sheet->frames.find(frameName); // 애니메이션 클립에서 따온 키가, 아틀라스 + json에 있는지 찾는거임
			if (itFrame == sheet->frames.end()) continue;

			//========================================================================

			const FrameData& frameData = itFrame->second; // 이름 피벗 렉트 들어있는 구조체임

			//if (renderComp->GetBitmap() != sheet->bitmap->Get()) // 지금 비트맵이랑 비교해서 다르면(로우 포인터 비교임)
			renderComp->SetBitmapResource(sheet->bitmap); // 아틀라스 변경

			renderComp->SetSourceRect(frameData.rect); // 부분랜더링 하는 상자임
			renderComp->pivot = frameData.pivot; // 해당 프레임의 피벗으로 변경해줌

		}
	}
};