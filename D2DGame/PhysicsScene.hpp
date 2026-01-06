//========================================================================
// PhysicsScene.hpp
//========================================================================

#pragma once

#include "SceneCore.hpp"

class PhysicsScene : public Scene {
private:

	RenderSystem* renderSystem = nullptr;
	CameraSystem camera;
	PhysicsSystemManager physics;
	TransformSystem* tfSys;
	DynamicMovementSystem* dySys;
	KinematicMovementSystem* kmSys;
	CollisionSystem* colSys;
	CollisionResolverSystem* colRes;
	CollisionEventSystem* colEvt;

	ID2D1SolidColorBrush* triggerBrush = nullptr;
	ID2D1SolidColorBrush* blockBrush = nullptr;
	ID2D1SolidColorBrush* enterBrush = nullptr;
	ID2D1SolidColorBrush* stayBrush = nullptr;
	ID2D1SolidColorBrush* exitBrush = nullptr;

	std::shared_ptr<GameObject> ob;
	std::shared_ptr<GameObject> npc;
	std::shared_ptr<GameObject> floor;
	std::shared_ptr<GameObject> box;
	std::shared_ptr<GameObject> wallA;
	std::shared_ptr<GameObject> wallB;
	std::shared_ptr<GameObject> box2;

public:
	~PhysicsScene() {
		delete renderSystem;
		delete tfSys;
		delete dySys;
		delete kmSys;
		delete colSys;
		delete colRes;
		delete colEvt;

		if (triggerBrush) triggerBrush->Release();
		if (blockBrush) blockBrush->Release();
		if (enterBrush) enterBrush->Release();
		if (stayBrush) stayBrush->Release();
		if (exitBrush) exitBrush->Release();
	}

	void Initialize() override {
		renderSystem = new RenderSystem{
		Direct2D::GetInstance().GetRenderTarget(),
		static_cast<float>(SCREEN_WIDTH),
		static_cast<float>(SCREEN_HEIGHT),
		};
		renderSystem->SetCamera(&camera);
		//========================================================================
		tfSys = new TransformSystem();
		dySys = new DynamicMovementSystem();
		kmSys = new KinematicMovementSystem();
		colSys = new CollisionSystem();
		colRes = new CollisionResolverSystem();
		colEvt = new CollisionEventSystem();

		physics.SetTransformSystem(tfSys);
		physics.SetDynamicMovementSystem(dySys);
		physics.SetKinematicMovementSystem(kmSys);
		physics.SetCollisionSystem(colSys);
		physics.SetCollisionResolverSystem(colRes);
		physics.SetCollisionEventSystem(colEvt);

		auto target = Direct2D::GetInstance().GetRenderTarget();

		target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LimeGreen), &triggerBrush); // 트리거
		target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &blockBrush);         // 충돌

		target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &enterBrush);      // onEnter
		target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Cyan), &stayBrush);         // onStay
		target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Magenta), &exitBrush);      // onExit

		//========================================================================

		ob = std::make_shared<GameObject>();
		auto tf2 = ob->GameObject::AddComponent<TransformComponent>();
		auto txt1 = ob->GameObject::AddComponent<TextRendererComponent>();

		tf2->SetPosition(20.0f, 20.0f); // 하드코딩임(충격)
		txt1->text = L"화살표로 이동 + 점프";
		txt1->fontSize = 28.0f;
		txt1->layoutSize = { 900,100 };
		txt1->color = D2D1::ColorF(D2D1::ColorF::White);

		AddObject(ob);

		//========================================================================
		CreateTestObjects();
		renderSystem->SetObjects(objects);
	}

	void Update(float delta) override {

		HandlePlayerInput();
		if (npc) {
			auto tf = npc->GetComponent<TransformComponent>();
			auto km = npc->GetComponent<KinematicBodyComponent>();

			if (tf && km) {
				float x = tf->position.x;
				if (x < 100.0f) { // 왼쪽 벽 근처
					km->direction.x = 1.0f;
				}
				else if (x > 1820.0f) { // 오른쪽 벽 근처 (1920 - 100)
					km->direction.x = -1.0f;
				}
			}
		}

		physics.Update(objects);
		for (auto& obj : objects) {
			auto flag = obj->GetComponent<CollisionFlagComponent>();
			auto txt = obj->GetComponent<TextRendererComponent>();
			if (!flag || !txt) continue;

			if (!flag->currentHits.empty() && flag->previousHits.empty())
				txt->text = L"Enter";
			else if (!flag->currentHits.empty())
				txt->text = L"Stay";
			else if (flag->previousHits.size() > 0 && flag->currentHits.empty())
				txt->text = L"Exit";
			else
				txt->text = L"";
		}
	}

	void HandlePlayerInput() {
		if (!box) return;

		auto dyn = box->GetComponent<DynamicBodyComponent>();
		if (!dyn) return;

		float moveForce = 500.0f;
		float jumpForce = -150000.0f;

		if (InputSystem::GetInstance().IsKeyDown(VK_LEFT)) {
			dyn->AddForce({ -moveForce, 0.0f });
		}
		if (InputSystem::GetInstance().IsKeyDown(VK_RIGHT)) {
			dyn->AddForce({ moveForce, 0.0f });
		}
		if (InputSystem::GetInstance().IsKeyPressed(VK_UP)) {
			// TODO: isGrounded 체크
			dyn->AddForce({ 0.0f, jumpForce });
		}
		if (InputSystem::GetInstance().IsKeyPressed(VK_DOWN)) {
			// TODO: isGrounded 체크
			dyn->AddForce({ 0.0f, moveForce });
		}
	}


	void Render() override {
		auto& d2d = Direct2D::GetInstance();
		d2d.BeginDraw();
		d2d.Clear(D2D1::ColorF(D2D1::ColorF::Black));

		renderSystem->Render();

		for (auto& obj : objects) {
			auto tf = obj->GetComponent<TransformComponent>();
			auto col = obj->GetComponent<ColliderComponent>();
			auto flag = obj->GetComponent<CollisionFlagComponent>();
			if (!tf || !col) continue;

			D2D1_RECT_F rect = col->GetAABB(tf->position);

			ID2D1Brush* brushToUse = nullptr;

			// 기본 구분: 트리거/블록
			brushToUse = col->isTrigger ? triggerBrush : blockBrush;

			if (flag) {
				if (!flag->currentHits.empty() && flag->previousHits.empty()) {
					brushToUse = enterBrush;
				}
				else if (flag->currentHits.empty() && !flag->previousHits.empty()) {
					brushToUse = exitBrush;
				}
				else if (!flag->currentHits.empty()) {
					brushToUse = stayBrush;
				}
			}

			Direct2D::GetInstance().GetRenderTarget()->DrawRectangle(rect, brushToUse, 6.0f);
		}

		d2d.EndDraw();
	}

	void OnEnter() override {
		InputEventSystem::GetInstance().onKeyPressed.Add(this, [this](int key) {
			if (key == VK_SPACE) { SceneManager::GetInstance().SetScene("Game"); }
			});
	}

	void OnExit() override {
		InputEventSystem::GetInstance().onKeyPressed.Remove(this);
	}
	void CreateTestObjects() {
		//===============================
		// 1. Static Block
		//===============================
		floor = std::make_shared<GameObject>();
		auto tf = floor->AddComponent<TransformComponent>();
		auto col = floor->AddComponent<ColliderComponent>();
		auto sp1 = floor->AddComponent<SpriteRendererComponent>();

		tf->SetPosition(960.0f, 1000.0f); // 화면 하단 중앙
		col->size = { 1920.0f, 100.0f };  // 화면 가로 전체
		col->isTrigger = false;
		col->isStatic = true;

		wallA = std::make_shared<GameObject>();
		auto tfA = wallA->AddComponent<TransformComponent>();
		auto colA = wallA->AddComponent<ColliderComponent>();
		auto sp1A = wallA->AddComponent<SpriteRendererComponent>();

		tfA->SetPosition(0.0f, 500.0f); // 화면 하단 중앙
		colA->size = { 100.0f, 1080.0f };  // 화면 가로 전체
		colA->isTrigger = false;
		colA->isStatic = true;

		wallB = std::make_shared<GameObject>();
		auto tfB = wallB->AddComponent<TransformComponent>();
		auto colB = wallB->AddComponent<ColliderComponent>();
		auto sp1B = wallB->AddComponent<SpriteRendererComponent>();

		tfB->SetPosition(1920.0f, 500.0f); // 화면 하단 중앙
		colB->size = { 100.0f, 1080.0f };  // 화면 가로 전체
		colB->isTrigger = false;
		colB->isStatic = true;

		AddObject(floor);
		AddObject(wallA);
		AddObject(wallB);

		//===============================
		// 2. 낙하 박스 - Dynamic
		//===============================
		box = std::make_shared<GameObject>();
		auto tf2 = box->AddComponent<TransformComponent>();
		auto dy = box->AddComponent<DynamicBodyComponent>();
		auto col2 = box->AddComponent<ColliderComponent>();
		auto sp2 = box->AddComponent<SpriteRendererComponent>();
		auto flag2 = box->AddComponent<CollisionFlagComponent>();

		tf2->SetPosition(640.0f, 200.0f); // 위에서 떨어지게
		col2->size = { 100.0f, 100.0f };
		col2->isTrigger = false;
		col2->isStatic = false;

		dy->useGravity = true;
		dy->gravityScale = 1.0f;
		dy->mass = 1.0f;

		AddObject(box);


		box2 = std::make_shared<GameObject>();
		auto tf22 = box2->AddComponent<TransformComponent>();
		auto dy2 = box2->AddComponent<DynamicBodyComponent>();
		auto col22 = box2->AddComponent<ColliderComponent>();
		auto sp22 = box2->AddComponent<SpriteRendererComponent>();
		auto flag22 = box2->AddComponent<CollisionFlagComponent>();

		tf22->SetPosition(640.0f, 400.0f); // 위에서 떨어지게
		col22->size = { 100.0f, 100.0f };
		col22->isTrigger = false;
		col22->isStatic = false;

		dy2->useGravity = true;
		dy2->gravityScale = 1.0f;
		dy2->mass = 0.5f;

		AddObject(box2);

		//===============================
		// 3. 좌우 움직이는 NPC - Kinematic + Trigger
		//===============================
		npc = std::make_shared<GameObject>();
		auto tf3 = npc->AddComponent<TransformComponent>();
		auto km = npc->AddComponent<KinematicBodyComponent>();
		auto col3 = npc->AddComponent<ColliderComponent>();
		auto flag = npc->AddComponent<CollisionFlagComponent>();
		auto sp3 = npc->AddComponent<SpriteRendererComponent>();

		tf3->SetPosition(200.0f, 600.0f); // 좌측에서 등장
		col3->size = { 80.0f, 80.0f };
		col3->isTrigger = true;
		col3->isStatic = false;

		km->speed = 100.0f;
		km->direction = { 1.0f, 0.0f }; // 오른쪽으로

		// 충돌 이벤트 출력


		AddObject(npc);
	}


};
