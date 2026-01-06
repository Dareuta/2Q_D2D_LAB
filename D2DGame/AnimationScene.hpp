//========================================================================
// AnimationScene.hpp
//========================================================================

#pragma once

#include "SceneCore.hpp"
#include "AnimationAssets.hpp"

class AnimationScene : public Scene {
private:
	RenderSystem* renderSystem = nullptr;
	CameraSystem camera;

	std::shared_ptr<GameObject> noel;
	std::shared_ptr<GameObject> txtObj1;
public:
	~AnimationScene() { delete renderSystem; }

	void Initialize() override {
		renderSystem = new RenderSystem{
		Direct2D::GetInstance().GetRenderTarget(),
		static_cast<float>(SCREEN_WIDTH),
		static_cast<float>(SCREEN_HEIGHT),
		};
		renderSystem->SetCamera(&camera);

		//========================================================================
		// 리소스 읽어오기

		auto& rm = ResourceManager::GetInstance();
		auto clipSet = rm.LoadClipSet(Noel::ClipSet);
		auto idle = rm.LoadSheetData(Noel::Idle.json, Noel::Idle.png);
		rm.LoadSheetData(Noel::Attack.json, Noel::Attack.png);
		rm.LoadSheetData(Noel::Special.json, Noel::Special.png);

		//========================================================================
		// 게임 오브젝트 생성

		noel = std::make_shared<GameObject>();
		auto tf = noel->AddComponent<TransformComponent>();
		auto sp = noel->AddComponent<SpriteRendererComponent>();
		auto cl = noel->AddComponent<AnimationClipComponent>();
		auto fr = noel->AddComponent<AnimationStateComponent>();

		tf->SetPosition(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);

		cl->sheetData = idle;
		cl->clips = clipSet;

		fr->SetClip(NoelClip::Idle); // idle

		fr->onClipFinished = [this]() {
			auto fr1 = noel->GetComponent<AnimationStateComponent>();
			auto cl1 = noel->GetComponent<AnimationClipComponent>();

			if (fr1->GetClip() == NoelClip::Attack1) {
				fr1->SetClip(NoelClip::Idle);
				cl1->sheetData = ResourceManager::GetInstance().LoadSheetData(Noel::Idle.json, Noel::Idle.png);
			}
			else if (fr1->GetClip() == NoelClip::Attack2) {
				fr1->SetClip(NoelClip::Idle);
				cl1->sheetData = ResourceManager::GetInstance().LoadSheetData(Noel::Idle.json, Noel::Idle.png);
			}
			else if (fr1->GetClip() == NoelClip::Special) {
				fr1->SetClip(NoelClip::Idle);
				cl1->sheetData = ResourceManager::GetInstance().LoadSheetData(Noel::Idle.json, Noel::Idle.png);
			}
			};

		//========================================================================
		AddObject(noel);

		txtObj1 = std::make_shared<GameObject>();
		auto tf2 = txtObj1->GameObject::AddComponent<TransformComponent>();
		auto txt1 = txtObj1->GameObject::AddComponent<TextRendererComponent>();

		tf2->SetPosition(20.0f, 20.0f); // 하드코딩임(충격)
		txt1->text = L"[SPACE: 씬전환 / A: 공격 / S: 중립 / D: 공격2 / F: 특수모션]";
		txt1->fontSize = 28.0f;
		txt1->layoutSize = { 900,100 };

		AddObject(txtObj1);

		//========================================================================
		renderSystem->SetObjects(objects);
	}

	void Update(float delta) override {
		auto& input = InputSystem::GetInstance();
		const float camSpeed = 500.0f * delta; // 카메라 이동속도
		if (input.IsKeyDown(VK_LEFT))  camera.MoveBy(-camSpeed, 0);
		if (input.IsKeyDown(VK_RIGHT)) camera.MoveBy(camSpeed, 0);
		if (input.IsKeyDown(VK_UP))    camera.MoveBy(0, -camSpeed);
		if (input.IsKeyDown(VK_DOWN))  camera.MoveBy(0, camSpeed);
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
			if (key == VK_SPACE) { SceneManager::GetInstance().SetScene("FSM"); }
			if (key == 'A') {
				auto fr = noel->GetComponent<AnimationStateComponent>();
				auto cl = noel->GetComponent<AnimationClipComponent>();
				fr->SetClip(NoelClip::Attack1);
				cl->sheetData = ResourceManager::GetInstance().LoadSheetData(Noel::Attack.json, Noel::Attack.png);
			};

			if (key == 'S') {
				auto fr = noel->GetComponent<AnimationStateComponent>();
				auto cl = noel->GetComponent<AnimationClipComponent>();
				fr->SetClip(NoelClip::Idle);

				cl->sheetData = ResourceManager::GetInstance().LoadSheetData(Noel::Idle.json, Noel::Idle.png);
			};

			if (key == 'D') {
				auto fr = noel->GetComponent<AnimationStateComponent>();
				auto cl = noel->GetComponent<AnimationClipComponent>();
				fr->SetClip(NoelClip::Attack2);

				cl->sheetData = ResourceManager::GetInstance().LoadSheetData(Noel::Attack.json, Noel::Attack.png);
			};

			if (key == 'F') {
				auto fr = noel->GetComponent<AnimationStateComponent>();
				auto cl = noel->GetComponent<AnimationClipComponent>();
				fr->SetClip(NoelClip::Special);

				cl->sheetData = ResourceManager::GetInstance().LoadSheetData(Noel::Special.json, Noel::Special.png);
			};
			});
	}

	void OnExit() override {
		InputEventSystem::GetInstance().onKeyPressed.Remove(this);
	}
};
