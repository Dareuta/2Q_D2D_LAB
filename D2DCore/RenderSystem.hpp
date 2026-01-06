//========================================================================
// RenderSystem.hpp
//========================================================================

#pragma once

#include <vector>
#include <algorithm>
#include <dwrite.h> //텍스트 렌더링 API
#include <wrl/client.h> // 릴리즈를 더 쉽게해준다고 함(아직 잘 모르겟슴)

#include "GameObject.hpp"
#include "TransformComponent.hpp"
#include "TextRendererComponent.hpp"
#include "SpriteRendererComponent.hpp"
#include "CameraSystem.hpp"

#include "BrushManager.hpp"
#include "TextFormatCache.hpp"

using Microsoft::WRL::ComPtr; // <wrl/client.h> 안에 있는거임

class RenderSystem { // 렌더 타겟 → 그릴 것(bitmap) → 변환 정보(transform) → Draw 호출
private:
	float screenWidth;
	float screenHeight;

	ID2D1HwndRenderTarget* renderTarget; // 순회하니까 계속 바뀌겠네 그럼

	std::vector<std::shared_ptr<GameObject>> allObjects;
	std::vector<std::shared_ptr<GameObject>> sortedObjects; // 정렬된 캐시
	bool needSort = true; // 더티 패턴	

	//각 랜더 시스템당, 자신의 카메라를 가짐 << 추후 변경 가능
	CameraSystem* camera = nullptr;

public:
	RenderSystem(ID2D1HwndRenderTarget* rt, float w, float h) :renderTarget(rt), screenWidth(w), screenHeight(h) {};

	void SetCamera(CameraSystem* cam) { camera = cam; }

	void RequireSort() { needSort = true; }

	void SetObjects(const std::vector<std::shared_ptr<GameObject>>& objects) { // 플래그 처리 때문에 생긴 함수임
		//여기다가 백터 비교하면 오히려 더 오래걸림
		allObjects = objects;
		sortedObjects = objects;
		needSort = true; // 새로 세팅되면 무조건 더티(정렬)

		for (auto obj : allObjects) {
			auto spr = obj->GetComponent<SpriteRendererComponent>();
			if (spr) {
				spr->onZOrderChanged = [this]() { // 델리게이트 사용되는 부분
					this->RequireSort();
					};
			}
		}
	}

	// 하나씩 출력하는거 * 전체 순환으로 분리하는게 성능이 좀 나쁘다고함
	//값 복사 안하고, 전체 순환시킴(복사하면 엄청 손해봄)
	//즉 이 함수 호출하면, 모든 등록된 게임오브젝트의 정보를 읽어서 그려줌 system이니까 가능		
	void Render() {
		RenderSprites();
		RenderText();
		renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	}

	void RenderSprites() {
		if (sortedObjects.empty()) return;
		//========================================================================
		// 먼저 정렬

		if (needSort) { // 캐시 되어있으면 건너뜀

			// 정렬 전에 둘 다 있는 객체만 필터링
			sortedObjects.erase(std::remove_if(sortedObjects.begin(), sortedObjects.end(), []
			(std::shared_ptr<GameObject> obj) {
					return !obj->enabled || // 랜더링 안하는건 뺌
						!obj->GetComponent<SpriteRendererComponent>() ||
						!obj->GetComponent<TransformComponent>();
				}), sortedObjects.end());

			std::sort(sortedObjects.begin(), sortedObjects.end(), [] //보기 더럽게 힘든 람다함수
			(std::shared_ptr<GameObject> a, std::shared_ptr<GameObject> b) {
					auto sa = a->GetComponent<SpriteRendererComponent>();
					auto sb = b->GetComponent<SpriteRendererComponent>();
					return sa->GetZOrder() < sb->GetZOrder();
				});

			needSort = false; //정렬완료
		}

		//========================================================================
		// 그리기

		for (auto obj : sortedObjects) { //등록된 모든 게임오브젝트를 순환(정확히는 매개변수로 넘겨받은 오브젝트들)

			if (!obj->enabled) continue;

			auto tf = obj->GetComponent<TransformComponent>();
			auto spr = obj->GetComponent <SpriteRendererComponent>(); //완전 유니티 같당

			if (!tf || !spr) continue;


			//컴포넌트 두개다 붙어있는데
			auto bmp = spr->GetBitmap();
			if (!bmp) continue;  // 비트맵을 가지고 있어야 그려짐, 하나라도 없으면 건너뛰는 구조

			//버퍼가 들어갈 자리임
			//renderTarget->SetTransform(tf->GetWorldMatrix()); //그림이 그려질 곳의 좌표 지정해줌 (카메라 좌표계 적용으로 변경)

			D2D1_MATRIX_3X2_F world = tf->GetWorldMatrix();

			if (camera) {//등록된 카메라가 있는경우
				world = world * camera->GetViewMatrix(screenWidth, screenHeight);
			}

			renderTarget->SetTransform(world);

			//배열에 들어있는 스케일값은 상대값임, 즉. 수치가 아니라 곱하는 값임

			//auto size = bmp->GetSize(); //ID2D1Bitmap 내부에 GetSize 있네 << 여기다가 곱해서 크기 구하는거임, 이미지 크기의 절대값임 
			// 마찬가지로, 부분 랜더링 구현으로 삭제

			// 사실상 버퍼임
			// + 컴포넌트 내부에 부분랜더링 크기를 지정해뒀음

			D2D1_RECT_F src = spr->GetSourceRect();
			float width = src.right - src.left;
			float height = src.bottom - src.top;

			//D2D1_RECT_F destination = D2D1::RectF(0, 0, src.right - src.left, src.bottom - src.top); //원점(좌 상단) 기준으로 보는거구나 오... 선형변환은 신이야
			float pivotX = spr->pivot.x; // 기본값 0.5 0.5인데, 필요하면 변경 가능함
			float pivotY = spr->pivot.y;

			D2D1_RECT_F destination = D2D1::RectF( //피벗 적용한 버전
				-width * pivotX, // 정규화된 값이라
				-height * pivotY,
				width * (1.0f - pivotX), // 1을 기준으로 빼주값을 빼주면 됨
				height * (1.0f - pivotY) // 약간 선형보간 느낌도 나네
			);

			renderTarget->DrawBitmap( //실질적으로 그리는건 이거로 하는거임, 위에서 처리한거 기반으로
				bmp,
				destination,
				1.0f, //투명도?
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, // 모드 대충 선형뭐시기 같은데(부드러운 선형 보간)
				&src // 이거 빠져서 안잘렸었음 // 클리핑 사각형 등록해야함
			);
		}

	}

	void RenderText() {
		auto* textBrush = BrushManager::GetInstance().GetWhiteBrush();
		if (!textBrush) return; //없으면 나가

		for (auto obj : allObjects) { // 모든 오브젝트를 순환함
			if (!obj->enabled) continue; // 출력안하는 플래그임

			auto tf = obj->GetComponent<TransformComponent>();
			auto txt = obj->GetComponent<TextRendererComponent>();
			if (!tf || !txt) continue; // 트랜스폼 + 텍스트랜더러 붙어있어야함

			std::wstring txtKey = txt->formatKeyOverride; // 키 이름 지정한게 있나?
			if (txtKey == L"") txtKey = (txt->font + L"_" + std::to_wstring((int)txt->fontSize)); //없다고하면, 일정한 규칙으로 만들어줌, "맑은 고딕_32" 이런식으로

			D2D1_MATRIX_3X2_F world = tf->GetWorldMatrix();
			if (!txt->screenSpace && camera) // 화면고정 아님 + 카메라 존재
				world = world * camera->GetViewMatrix(screenWidth, screenHeight);
			renderTarget->SetTransform(world); // 랜더타겟 좌표 지정

			textBrush->SetColor(txt->color);

			auto format = TextFormatCache::GetInstance().GetFormat(txtKey, txt->fontSize, txt->font); //캐싱해서 포맷 가져옴
			if (!format) return; //없으면 나가라

			D2D1_RECT_F layout = D2D1::RectF(0, 0, txt->layoutSize.width, txt->layoutSize.height);

			renderTarget->DrawTextW(
				txt->text.c_str(), // C스타일로 문자열 반환하고
				static_cast<UINT32>(txt->text.length()), // 문자길이 계산하고
				format, //위에서 만든 포멧 넣고
				&layout, // 컴포넌트가 가지고있는 패널크기 넣고
				textBrush // 브러-쉬는 브-러쉬임 브러쉬 비쌈 아무튼 비쌈
			);
		}
	}

	/* // 텍스트 포맷 캐시 추가 및, 브러쉬 매니저 추가로 주석처리함
	void RenderText() {
		//========================================================================
		// 텍스트 출력

		static IDWriteFactory* dwriteFactory = nullptr; //static쓰는 이유는, IDW뭐시깽이 무거워서 그렇다고 함, 여러개 만들면 힘듬

		if (!dwriteFactory) { // null일경우, 즉 생성이 안된 경우 << 초기화?인듯
			DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED, //공유 팩토리. 시스템 전체에서 공용으로 사용되는 설정(폰트, 캐시같은거) 공유

				__uuidof(IDWriteFactory), // 이게 뭔 거지같은거냐, GUID를 컴파일 타임에서 얻어내는 키워드임
				// 만약 하드 코딩한다면 {B859EE5A-D838-4B5B-A2F4-7D6F4D6E5CA8} 같은 끔찍한걸 넣어야함
				// GUID? COM 인터페이스의 고유 ID임

				reinterpret_cast<IUnknown**>(&dwriteFactory) //IDWriteFactory << IUnknown이 부모임
				//안에서 내용물 담아달라고 건네주는거라 **임, 우리가 필요한건 *라서
			);
		} //아무튼 여기까지가 싱글톤처럼 한번만 생성하는거임

		ID2D1SolidColorBrush* textBrush = nullptr;
		renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &textBrush); //브러쉬 담음
		//색상도 "객체"로 만들어서 넘겨야함, 맙소사, Release() 해줘야함 !!!중요!!!


		for (auto obj : allObjects) {

			if (!obj->enabled) continue;

			auto tf = obj->GameObject::GetComponent<TransformComponent>();
			auto txt = obj->GameObject::GetComponent<TextRendererComponent>();

			if (!tf || !txt) continue;

			IDWriteTextFormat* format = nullptr; //텍스트의 스타일(폰트, 크기, 정렬같은거) 정의하는 객체
			//!!이거도 릴!리!즈! 해야함!


			dwriteFactory->CreateTextFormat( //확실히 팩토리가 뚝딱뚝딱 만들어주넹 신기하다
				L"맑은 고딕",				// 폰트이름임, 무-려 한글로 넣어도 인식되네
				nullptr,					// 고-급 기능, 우리같은 사람들은 안씀 << 직접 커스텀 폰트 파일을 컬렉션으로 쓴다고함(진짜모름)
				DWRITE_FONT_WEIGHT_NORMAL,	// 굵기, 볼드체 같은거
				DWRITE_FONT_STYLE_NORMAL,	// 기울임 여부, 이텔릭 같은거
				DWRITE_FONT_STRETCH_NORMAL, // 늘이기, 수평으로 늘려준다고 하는데, 폰트에서 지원해주는 경우에만 사용가능한 옵션
				// 아무튼 전부 기본값(normal)쓰는거임
				txt->fontSize,				// 폰트크기(컴포넌트 내부에 있는 사이즈 읽어와서 씀)
				L"ko-kr",					// 로?케일, 한글 깨짐 방지라고 함, 언어별 설정이라고 함
				&format						// 설정 완성해서 여기 담아달라는거
			);

			// 대충 좌표계산하는거, 많이봤던거네
			D2D1_MATRIX_3X2_F world = tf->GetWorldMatrix();
			if (!txt->screenSpace  && camera) world = world * camera->GetViewMatrix(screenWidth, screenHeight);
			renderTarget->SetTransform(world);

			textBrush->SetColor(txt->color); // 색깔을 여기서 바꾸네? 오, 생성할때는 기본값 넣어주고
			//이거, 브러쉬 하나 가지고, 색 바꾸면서 여러번 쓰는거임 << 이게 메모리 효율에 좋다고함

			D2D1_RECT_F layout = D2D1::RectF(0, 0, 2000, 200); // 텍스트가 출력될 범위임 << 나중에 잘 봐둬야할듯 << 나중에 크기 변경하는것도 추가해야함
			//좌상단(0,0)기준으로 출력되는데, 결국 좌표에 따라 이동됨

			renderTarget->DrawTextW(
				txt->text.c_str(),						 // 문자열
				static_cast<UINT32>(txt->text.length()), // 전체 문자 수
				format,			// 위에서 설정한 내용
				&layout,		// 랜더링할 범위
				textBrush		// 브-러쉬
			);

			if (format) format->Release(); //릴!!!리!!!즈!!!
		}

		renderTarget->SetTransform(D2D1::Matrix3x2F::Identity()); // 랜더타겟 비우는거임
		if (textBrush) textBrush->Release(); //릴!리!즈!
	}
	*/
};