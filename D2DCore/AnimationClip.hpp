//========================================================================
// AnimationClip.hpp
//========================================================================

#pragma once

#include <string>
#include <vector>

//!!주의!!! 키(이름)이 무조건 FrameData내에 있는 이름과 1대1 대응해야함!!!!

// 이전에 framedata.hpp 에서 정리된건 스프라이트 + json이라면
// 이곳에서 정리된건, 애니메이션에 대한 구조체임,
// 즉, FrameData + AnimationCLip을 합쳐서 사용해야 애니메이션이 굴러감

struct AnimationFrame {
	std::wstring spriteName; // 흐음....FrameData 내부에 있는 json에서 파싱해온 이름과 동일하게 관리해야하는 부분인데 어떻게함?
							 //언오더 맵에서 같은 키를 사용해야함, 중요함, 꼭꼭 일치시켜야 함

	float duration; // 이거 사실 동일하게 처리해도 문제는 없긴한데, 흠. 일단 놔두자
	// 사실상, 프레임에 대한 정보를 추가로 담는거임, 단 이번에는 애니메이션과 관련된 추가정보일 뿐
};

struct AnimationClip { 
	std::wstring clipName; // 여기가 이제, 애니메이션 단위(Idle Run Walk 등)일꺼임.
	std::vector<AnimationFrame> frames; // 정리해둔 데이터를 순서가 있는 vector로 정리함, 
										// 즉, 애니메이션 프레임에 있는 이름(키)를 기반으로, FrameData 구조체 안에 해싱된 정보를 읽어와서 처리하는 구조일꺼임
	bool looping = true; // 반복 여부, 기본값 true 
};