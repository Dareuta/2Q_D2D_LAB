//========================================================================
// VRAMTestScene
//========================================================================

#pragma once

#include "SceneCore.hpp"
#include "random"

#include <psapi.h>
#include <sstream>
#include <iomanip>

#include <dxgi1_6.h> // DXGI_QUERY_VIDEO_MEMORY_INFO
#pragma comment(lib, "dxgi.lib")

class VRAMScene : public Scene {
private:
	RenderSystem* renderSystem = nullptr;
	CameraSystem camera;

	std::shared_ptr<GameObject> txtObj = nullptr;
	std::shared_ptr<GameObject> ramTxt = nullptr;
	std::shared_ptr<GameObject> vramTxt = nullptr;

	//	std::shared_ptr<BitmapResource> testImage; // 컴포넌트 소유로 변경함

	ComPtr<IDXGIAdapter3> dxgiAdapter;

public:
	~VRAMScene() { delete renderSystem; }

	void Initialize() override {

		ComPtr<IDXGIFactory6> factory;
		CreateDXGIFactory(IID_PPV_ARGS(&factory));

		factory->EnumAdapters1(0, reinterpret_cast<IDXGIAdapter1**>(dxgiAdapter.GetAddressOf()));

		renderSystem = new RenderSystem(
			Direct2D::GetInstance().GetRenderTarget(),
			static_cast<float>(SCREEN_WIDTH),
			static_cast<float>(SCREEN_HEIGHT)
		);
		renderSystem->SetCamera(&camera);

		//testImage = ResourceManager::GetInstance().LoadBitmapResource(L"assets/test.png"); //내부에서 직접 호출해서 여러번 반복하도록 변경

		txtObj = std::make_shared<GameObject>();
		auto tf2 = txtObj->GameObject::AddComponent<TransformComponent>(); // 나중에 트랜스폼은 강제로 넣게 바꿔야긋다 어차피 필수인데
		auto txt1 = txtObj->GameObject::AddComponent<TextRendererComponent>();

		tf2->SetPosition(20.0f, 20.0f); // 하드코딩임(충격)
		txt1->text = L"[SPACE: 씬전환 / A: 생성 / S: 로우포인터 방식으로 생성 / D: 삭제]";
		txt1->fontSize = 28.0f;
		txt1->layoutSize = { 900,100 };

		AddObject(txtObj);

		// [1] RAM 텍스트
		ramTxt = std::make_shared<GameObject>();
		auto tfRAM = ramTxt->AddComponent<TransformComponent>();
		auto txtRAM = ramTxt->AddComponent<TextRendererComponent>();
		tfRAM->SetPosition(20.0f, 70.0f);
		txtRAM->fontSize = 28.0f;
		txtRAM->layoutSize = { 900,100 };
		txtRAM->color = D2D1::ColorF(D2D1::ColorF::Green);
		txtRAM->text = L"[RAM 정보]"; // 초기 텍스트
		AddObject(ramTxt);

		// [2] VRAM 텍스트
		vramTxt = std::make_shared<GameObject>();
		auto tfVRAM = vramTxt->AddComponent<TransformComponent>();
		auto txtVRAM = vramTxt->AddComponent<TextRendererComponent>();
		tfVRAM->SetPosition(20.0f, 120.0f);
		txtVRAM->fontSize = 28.0f;
		txtVRAM->layoutSize = { 900,100 };
		txtVRAM->color = D2D1::ColorF(D2D1::ColorF::Cyan);
		txtVRAM->text = L"[VRAM 정보]";
		AddObject(vramTxt);

		renderSystem->SetObjects(objects);
	}
	//========================================================================
	// 컴퓨터 괴롭히는 코드

	void AddRandomObjects(const std::wstring& path) {
		auto testImage = ResourceManager::GetInstance().LoadBitmapResource(path); //매번 경로를 읽어옴
		if (!testImage || !testImage->Get()) return; // 읽어오기 실패시 리턴

		int num = 10; // 10개씩 팍팍 만들자

		for (int i = 0; i < num; ++i) {
			auto obj = CreateRandomSpriteObject(testImage, &camera);
			AddObject(obj);
		}

		renderSystem->SetObjects(objects);
	}

	void RemoveRandomObjects() {		
		for (size_t i = 3; i < objects.size(); ++i) {
			objects[i]->enabled = false; //shared_ptr이라서, enabled처리만 함
		}
	}

	std::shared_ptr<GameObject> CreateRandomSpriteObject(const std::shared_ptr<BitmapResource>& res, CameraSystem* cam) {
		static std::random_device rd; //스테틱이라, 여러번 호출되도 한번만 가질껄
		static std::mt19937 rng(rd());
		static std::uniform_real_distribution<float> dist(-500.0f, 500.0f); //랜덤 범위

		auto obj = std::make_shared<GameObject>(); // 오브젝트 생성해서

		auto tf = obj->AddComponent<TransformComponent>(); //컴포넌트 붙이고
		auto spr = obj->AddComponent<SpriteRendererComponent>();

		spr->SetBitmapResource(res); // 입력받은 bmp받아서
		//소스렉터 지정 안하면 알아서 이미지 크기로 잘라서 출력해줌

		float x = cam->GetPosition().x + (SCREEN_WIDTH / 2.0f) + dist(rng);
		float y = cam->GetPosition().y + (SCREEN_HEIGHT / 2.0f) + dist(rng);
		tf->SetPosition(x, y);

		return obj;
	}

	//========================================================================

	std::wstring GetVRAMInfo() {
		if (!dxgiAdapter) return L"VRAM: [어댑터 없음]";

		DXGI_QUERY_VIDEO_MEMORY_INFO info = {};
		dxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);

		double usageMB = info.CurrentUsage / (1024.0 * 1024.0);
		double budgetMB = info.Budget / (1024.0 * 1024.0);

		std::wstringstream ss;
		ss << std::fixed << std::setprecision(2);
		ss << L"VRAM: " << usageMB << L" MB / " << budgetMB << L" MB";
		return ss.str();
	}

	std::wstring GetMemoryInfo() {
		std::wstringstream ss;
		ss << GetRAMInfo() << L" | " << GetVRAMInfo();
		return ss.str();
	}

	std::wstring GetRAMInfo() {
		PROCESS_MEMORY_COUNTERS_EX pmc;
		if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
			double ramMB = pmc.WorkingSetSize / (1024.0 * 1024.0);
			std::wstringstream ss;
			ss << std::fixed << std::setprecision(2);
			ss << L"RAM: " << ramMB << L" MB";
			return ss.str();
		}
		return L"RAM: [조회 실패]";
	}


	//테스트 임시코드(이것도 gpt한테 짜라고 했음)
	//주의, 이거로 만든 오브젝트들 터짐(비교용이라 놔둠)
	void AddUnsharedRandomObjects(const std::wstring& path) {
		static std::random_device rd;
		static std::mt19937 rng(rd());
		static std::uniform_real_distribution<float> dist(-500.0f, 500.0f);

		auto bmp = ResourceManager::GetInstance().LoadBitmapRaw(path);

		if (!bmp) return;

		for (int i = 0; i < 10; ++i) {
			auto obj = std::make_shared<GameObject>();
			auto tf = obj->AddComponent<TransformComponent>();
			auto spr = obj->AddComponent<SpriteRendererComponent>();

			spr->SetBitmapResource(std::make_shared<BitmapResource>(bmp)); // 강제로 매번 새 Bitmap으로 감싸줌
			tf->SetPosition(camera.GetPosition().x + dist(rng) + (SCREEN_WIDTH / 2.0f), camera.GetPosition().y + dist(rng) + (SCREEN_HEIGHT / 2.0f));

			AddObject(obj);
		}
		renderSystem->SetObjects(objects);
	}

	//========================================================================

	void Update(float delta) override {
		auto& input = InputSystem::GetInstance();
		const float camSpeed = 300.0f * delta;
		if (input.IsKeyDown(VK_LEFT))  camera.MoveBy(-camSpeed, 0);
		if (input.IsKeyDown(VK_RIGHT)) camera.MoveBy(camSpeed, 0);
		if (input.IsKeyDown(VK_UP))    camera.MoveBy(0, -camSpeed);
		if (input.IsKeyDown(VK_DOWN))  camera.MoveBy(0, camSpeed);


		if (objects.size() >= 3) {
			auto ramTxt = objects[1]->GetComponent<TextRendererComponent>();
			auto vramTxt = objects[2]->GetComponent<TextRendererComponent>();
			if (ramTxt) ramTxt->text = GetRAMInfo();
			if (vramTxt) vramTxt->text = GetVRAMInfo();
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
		InputEventSystem::GetInstance().onKeyPressed.Add(this, [this](int key) {
			if (key == VK_SPACE) { SceneManager::GetInstance().SetScene("Animation"); }
			if (key == 'A') { this->AddRandomObjects(L"assets/test.png"); }
			if (key == 'D') { this->RemoveRandomObjects(); }
			//if (key == 'S') { this->AddUnsharedRandomObjects(L"assets/test.png"); } // 테스트코드 주석처리해둠
			});
	}

	void OnExit() override {
		InputEventSystem::GetInstance().onKeyPressed.Remove(this);
	}
};