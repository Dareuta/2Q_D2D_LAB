//========================================================================
// GameScene.hpp
//========================================================================

#pragma once

#include "SceneCore.hpp"

#define OB1_ORBIT_RADIUS 100.0f // 사실 안씀
#define OB2_ORBIT_RADIUS 300.0f
#define OB3_ORBIT_RADIUS 300.0f 
#define OB4_ORBIT_RADIUS 300.0f

// 알아둬야 하는게, 아직 new로 생성한 게임오브젝트를 DELETE하지 않고있음
// 나중에 처리해야함 기억해둘것

class GameScene : public Scene {
private:
	RenderSystem* renderSystem = nullptr;
	CameraSystem camera;
	TransformSystem transformSystem;

	std::shared_ptr<GameObject> object1 = nullptr; // 태양
	std::shared_ptr<GameObject> object2 = nullptr; // 지구
	std::shared_ptr<GameObject> object3 = nullptr; // 달
	std::shared_ptr<GameObject> object4 = nullptr; // 그리고 무언가
	std::shared_ptr<GameObject> background = nullptr; // 배경
	std::shared_ptr<GameObject> txtObj = nullptr;

	float object1OrbitAngle = 0.0f; // 공전
	float object2OrbitAngle = 0.0f;
	float object3OrbitAngle = 0.0f;
	float object4OrbitAngle = 0.0f;

	float object1SelfRotation = 0.0f; // 자전
	float object2SelfRotation = 0.0f;
	float object3SelfRotation = 0.0f;
	float object4SelfRotation = 0.0f;

	// 이거 지역변수로 가지고있으면 터짐
	//std::shared_ptr<BitmapResource> imagineDragons;
	//std::shared_ptr<BitmapResource> bgDragons;
	// 컴포넌트에 넣어서 처리함

public:
	~GameScene() {
		delete renderSystem;
	}

	void Initialize() override {
		//========================================================================
		// 기본적인 등록		

		renderSystem = new RenderSystem{ // 랜더시스템 객체 생성
			Direct2D::GetInstance().GetRenderTarget(), //타겟
			static_cast<float>(SCREEN_WIDTH),	//화면 크기
			static_cast<float>(SCREEN_HEIGHT),	//인듯
		};
		renderSystem->SetCamera(&camera); // 카메라 등록

		auto& rm = ResourceManager::GetInstance(); //리소스 매니저로 읽어올꺼임

		//========================================================================
		// 리소스 경로

		// 리소스 매니저 변경에 따른, 주석처리
		//ID2D1Bitmap* object1to4Image = rm.LoadBitmapFromFile(L"assets/KessokuBand.png");
		//ID2D1Bitmap* bgImage = rm.LoadBitmapFromFile(L"assets/BG.png");
		//if (!bgImage || !object1to4Image) return;

		//이메진 드래곤 노래 좋아요
		auto object1to4Image = rm.LoadBitmapResource(L"assets/KessokuBand.png"); // 리소스 로드하는거, shared_ptr로 나올껄?
		auto bgImage = rm.LoadBitmapResource(L"assets/BG.png");

		if (!object1to4Image || !bgImage) return; // 나가라 넌


		// 게임오브젝트 등록
		//========================================================================
		// object1 ///////////////////////////////////////////////////////////////
		//========================================================================

		object1 = std::make_shared<GameObject>();
		auto tf1 = object1->AddComponent<TransformComponent>();
		auto spr1 = object1->AddComponent<SpriteRendererComponent>();

		spr1->SetBitmapResource(object1to4Image);
		spr1->SetSourceRect(D2D1::RectF(0, 0, 350, 400));

		tf1->SetPosition(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

		AddObject(object1);

		//========================================================================
		// object2 ///////////////////////////////////////////////////////////////
		//========================================================================

		object2 = std::make_shared<GameObject>();
		auto tf2 = object2->AddComponent<TransformComponent>();
		auto spr2 = object2->AddComponent<SpriteRendererComponent>();

		spr2->SetBitmapResource(object1to4Image);
		spr2->SetSourceRect(D2D1::RectF(0, 400, 350, 800));

		tf2->SetParent(tf1);

		AddObject(object2);

		//========================================================================
		// object3 ///////////////////////////////////////////////////////////////
		//========================================================================

		object3 = std::make_shared<GameObject>();
		auto tf3 = object3->AddComponent<TransformComponent>();
		auto spr3 = object3->AddComponent<SpriteRendererComponent>();

		spr3->SetBitmapResource(object1to4Image);
		spr3->SetSourceRect(D2D1::RectF(350, 0, 750, 400));

		tf3->SetParent(tf2);

		AddObject(object3);

		//========================================================================
		// object4 ///////////////////////////////////////////////////////////////
		//========================================================================

		object4 = std::make_shared<GameObject>();
		auto tf4 = object4->AddComponent<TransformComponent>();
		auto spr4 = object4->AddComponent<SpriteRendererComponent>();

		spr4->SetBitmapResource(object1to4Image);
		spr4->SetSourceRect(D2D1::RectF(350, 400, 750, 800));

		tf4->SetParent(tf3);

		AddObject(object4);

		//========================================================================
		// background ////////////////////////////////////////////////////////////
		//========================================================================

		background = std::make_shared<GameObject>();
		auto bgTf = background->AddComponent<TransformComponent>();
		auto bgSpr = background->AddComponent<SpriteRendererComponent>();

		bgSpr->SetBitmapResource(bgImage);
		bgSpr->SetZOrder(-100);// 가장 뒤

		tf1->SetPosition(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

		AddObject(background);

		//========================================================================

		txtObj = std::make_shared<GameObject>();

		auto tf23 = txtObj->GameObject::AddComponent<TransformComponent>(); // 나중에 트랜스폼은 강제로 넣게 바꿔야긋다 어차피 필수인데
		auto txt = txtObj->GameObject::AddComponent<TextRendererComponent>();

		tf23->SetPosition(SCREEN_WIDTH / 2.0f - 260.0f, SCREEN_HEIGHT / 2.0f + 240.0f); // 하드코딩임(충격)
		txt->text = L"[SPACE키를 눌러 Scene을 전환합니다.]";
		txt->fontSize = 32.0f;
		txt->color = D2D1::ColorF(D2D1::ColorF::White);

		AddObject(txtObj);

		renderSystem->SetObjects(objects);
	}

	void Update(float delta) override {
		auto& input = InputSystem::GetInstance();

		//카메라 이동
		//========================================================================
		const float camSpeed = 500.0f * delta; // 카메라 이동속도
		if (input.IsKeyDown(VK_LEFT))  camera.MoveBy(-camSpeed, 0);
		if (input.IsKeyDown(VK_RIGHT)) camera.MoveBy(camSpeed, 0);
		if (input.IsKeyDown(VK_UP))    camera.MoveBy(0, -camSpeed);
		if (input.IsKeyDown(VK_DOWN))  camera.MoveBy(0, camSpeed);

		if (input.IsKeyDown(VK_ESCAPE)) { // ESC누르면 씬 전환
			//SceneManager::GetInstance().SetScene("Title"); //델리게이트 방식으로 변경함
			//return;
		}
		//========================================================================
		// 공전 자전 갱신

		object1OrbitAngle += 1.0f * delta;
		object2OrbitAngle += 1.0f * delta;
		object3OrbitAngle += 1.0f * delta;
		object4OrbitAngle += 10.0f * delta;

		object1SelfRotation += 2.0f * delta;
		object2SelfRotation += 1.0f * delta;
		object3SelfRotation += 1.0f * delta;
		object4SelfRotation += -10.0f * delta;

		// object1
		constexpr float pi = 3.14159f;

		auto tf1 = object1->GetComponent<TransformComponent>();
		tf1->SetRotation(object1SelfRotation * 180.0f / pi);

		// object2
		auto tf2 = object2->GetComponent<TransformComponent>();
		float x2 = cosf(object2OrbitAngle) * OB2_ORBIT_RADIUS;
		float y2 = sinf(object2OrbitAngle) * OB2_ORBIT_RADIUS;
		tf2->SetPosition(x2, y2);
		tf2->SetRotation(object2SelfRotation * 180.0f / pi);

		// object3
		auto tf3 = object3->GetComponent<TransformComponent>();
		float x3 = cosf(object3OrbitAngle) * OB3_ORBIT_RADIUS;
		float y3 = sinf(object3OrbitAngle) * OB3_ORBIT_RADIUS;
		tf3->SetPosition(x3, y3);
		tf3->SetRotation(object3SelfRotation * 180.0f / pi);

		// object4
		auto tf4 = object4->GetComponent<TransformComponent>();
		float x4 = cosf(object4OrbitAngle) * OB4_ORBIT_RADIUS;
		float y4 = sinf(object4OrbitAngle) * OB4_ORBIT_RADIUS;
		tf4->SetPosition(x4, y4);
		tf4->SetRotation(object4SelfRotation * 180.0f / pi);

		transformSystem.Update(objects);
	}

	void Render() override {

		auto& d2d = Direct2D::GetInstance();

		d2d.BeginDraw();
		d2d.Clear(D2D1::ColorF(D2D1::ColorF::DarkBlue));

		renderSystem->Render();

		d2d.EndDraw();
	}

	void OnEnter() override {
		InputEventSystem::GetInstance().onKeyPressed.Add(this, [](int key) {
			if (key == VK_SPACE) { SceneManager::GetInstance().SetScene("VRAM"); }
			});
	}

	void OnExit() override {
		InputEventSystem::GetInstance().onKeyPressed.Remove(this);
	}
};
