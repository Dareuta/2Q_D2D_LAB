//========================================================================
// FSMScene.hpp
//========================================================================

#pragma once

#include "SceneCore.hpp"
#include "AnimationAssets.hpp"
#include "FSMNoel.hpp"

class FSMScene : public Scene {
private:
	RenderSystem* renderSystem = nullptr;
	CameraSystem camera;

	std::shared_ptr<GameObject> ob;
	std::shared_ptr<GameObject> noel;

public:
	~FSMScene() { delete renderSystem; }

	void Initialize() override {
		renderSystem = new RenderSystem{
		Direct2D::GetInstance().GetRenderTarget(),
		static_cast<float>(SCREEN_WIDTH),
		static_cast<float>(SCREEN_HEIGHT),
		};
		renderSystem->SetCamera(&camera);

		//========================================================================

		ob = std::make_shared<GameObject>();
		auto tf2 = ob->GameObject::AddComponent<TransformComponent>();
		auto txt1 = ob->GameObject::AddComponent<TextRendererComponent>();

		tf2->SetPosition(20.0f, 20.0f); // 하드코딩임(충격)
		txt1->text = L"아랫방향키 착지, 방향키, A, S";
		txt1->fontSize = 28.0f;
		txt1->layoutSize = { 900,100 };
		txt1->color = D2D1::ColorF(D2D1::ColorF::White);

		AddObject(ob);

		//========================================================================
		auto& rm = ResourceManager::GetInstance();
		auto clipSet = rm.LoadClipSet(Noel::ClipSet);
		auto idle = rm.LoadSheetData(Noel::Idle.json, Noel::Idle.png);
		rm.LoadSheetData(Noel::Attack.json, Noel::Attack.png);
		rm.LoadSheetData(Noel::Move.json, Noel::Move.png);
		rm.LoadSheetData(Noel::Jump.json, Noel::Jump.png);
		rm.LoadSheetData(Noel::Special.json, Noel::Special.png);
		rm.LoadSheetData(Noel::Change.json, Noel::Change.png);

		//========================================================================

		noel = std::make_shared<GameObject>();
		auto tf = noel->GameObject::AddComponent<TransformComponent>();
		auto sp = noel->GameObject::AddComponent<SpriteRendererComponent>();
		auto cl = noel->GameObject::AddComponent<AnimationClipComponent>();
		auto st = noel->GameObject::AddComponent<AnimationStateComponent>();
		auto fs = noel->GameObject::AddComponent<FSMComponent>();
		fs->RegisteSystem(std::make_unique<FSMNoel>());
		fs->SetState(L"Idle");

		tf->SetPosition(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

		cl->sheetData = idle;
		cl->clips = clipSet;

		st->SetClip(NoelClip::Idle); // idle

		AddObject(noel);

		//========================================================================

		renderSystem->SetObjects(objects);
	}

	void Update(float delta) override {
		auto fsmCom = noel->GetComponent<FSMComponent>();
		auto& input = InputSystem::GetInstance();

		if (fsmCom) {
			if (input.IsKeyDown(VK_UP) && input.IsKeyDown(VK_RIGHT)) { fsmCom->inputState = InputState::RightUp; }
			else if (input.IsKeyDown(VK_UP) && input.IsKeyDown(VK_LEFT)) { fsmCom->inputState = InputState::LeftUp; }
			else if (input.IsKeyDown(VK_UP)) { fsmCom->inputState = InputState::Up; }
			else if (input.IsKeyDown(VK_RIGHT)) { fsmCom->inputState = InputState::Right; }
			else if (input.IsKeyDown(VK_LEFT)) { fsmCom->inputState = InputState::Left; }
			else if (input.IsKeyDown(VK_DOWN)) { fsmCom->inputState = InputState::Down; }
			else if (input.IsKeyDown('A')) { fsmCom->inputState = InputState::KeyA; }
			else if (input.IsKeyDown('S')) { fsmCom->inputState = InputState::KeyB; }
			else { fsmCom->inputState = InputState::Default; }
		}


		// 임의로 업데이트 돌려줌
		auto fsm = noel->GetComponent<FSMComponent>();
		if (fsm) { fsm->Update(delta); }
	}

	void Render() override {
		auto& d2d = Direct2D::GetInstance();
		d2d.BeginDraw();
		d2d.Clear(D2D1::ColorF(D2D1::ColorF::Black));

		renderSystem->Render();
		d2d.EndDraw();
	}

	void OnEnter() override {

		AnimationSystem::GetInstance().SetObjects(objects);

		InputEventSystem::GetInstance().onKeyPressed.Add(this, [this](int key) {
			if (key == VK_SPACE) { SceneManager::GetInstance().SetScene("Title"); }
			});

		auto fs = noel->GetComponent<FSMComponent>();
		auto fr = noel->GetComponent<AnimationStateComponent>();

		if (fs) {
			fs->SetState(L"Idle");
			fs->Enter();
		}
	}

	void OnExit() override {
		auto fr = noel->GetComponent<AnimationStateComponent>();

		if (fr) {
			fr->onClipFinished = nullptr; // 콜백 해제		
		}

		InputEventSystem::GetInstance().onKeyPressed.Remove(this);
	}
};
