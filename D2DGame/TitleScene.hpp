//========================================================================
// TitleScene.hpp
//========================================================================

#pragma once

#include "SceneCore.hpp"

class TitleScene : public Scene {
private:
	RenderSystem* renderSystem = nullptr;
	std::shared_ptr<GameObject> txtObj;

public:
	~TitleScene() { delete renderSystem; }

	void Initialize() override {
		// 이미지 로드함
		auto& rm = ResourceManager::GetInstance();
		//ID2D1Bitmap* titleImage = rm.LoadBitmapFromFile(L"assets/Title.png"); // 마찬가지로, 리소스매니저 수정으로 주석처리함

		auto titleImage = rm.LoadBitmapResource(L"assets/Title.png");

		if (!titleImage) return; // 나가잇


		// 랜더 시스템 설정

		renderSystem = new RenderSystem(
			Direct2D::GetInstance().GetRenderTarget(),
			static_cast<float>(SCREEN_WIDTH),
			static_cast<float>(SCREEN_HEIGHT)
		);

		// 1. 이미지 오브젝트 등록

		if (titleImage) {
			auto imgObj = std::make_shared<GameObject>();
			auto tf = imgObj->GameObject::AddComponent<TransformComponent>();
			auto spr = imgObj->GameObject::AddComponent<SpriteRendererComponent>();

			spr->SetBitmapResource(titleImage);
			auto size = titleImage->Get()->GetSize();
			tf->SetPosition(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
			spr->SetSourceRect(D2D1::RectF(0, 0, size.width, size.height));

			AddObject(imgObj);
		}

		txtObj = std::make_shared<GameObject>();

		auto tf2 = txtObj->GameObject::AddComponent<TransformComponent>(); // 나중에 트랜스폼은 강제로 넣게 바꿔야긋다 어차피 필수인데
		auto txt = txtObj->GameObject::AddComponent<TextRendererComponent>();

		tf2->SetPosition(SCREEN_WIDTH / 2.0f - 260.0f, SCREEN_HEIGHT / 2.0f + 240.0f); // 하드코딩임(충격)
		txt->text = L"[SPACE키를 눌러 Scene을 전환합니다.]";
		txt->fontSize = 32.0f;
		txt->color = D2D1::ColorF(D2D1::ColorF::White);

		AddObject(txtObj);

		renderSystem->SetObjects(objects);

	}

	void Update(float delta) override {
		if (InputSystem::GetInstance().IsKeyPressed(VK_SPACE)) {
			//SceneManager::GetInstance().SetScene("Game"); //델리게이트 방식으로 변경함
		}
	}

	void Render() override {
		auto& d2d = Direct2D::GetInstance();
		d2d.BeginDraw();
		d2d.Clear(D2D1::ColorF(D2D1::ColorF::Black));

		renderSystem->Render();
		d2d.EndDraw();
	}

	void OnEnter() override {
		InputEventSystem::GetInstance().onKeyPressed.Add(this, [](int key) {
			if (key == VK_SPACE) { SceneManager::GetInstance().SetScene("Physics"); }
			});
	}

	void OnExit() override {
		InputEventSystem::GetInstance().onKeyPressed.Remove(this);
	}
};

