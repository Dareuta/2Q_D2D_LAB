//========================================================================
// ResourceManager.hpp
//========================================================================

#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <d2d1.h>
#include <wincodec.h>

#include "Singleton.hpp"
#include "BitmapResource.hpp"
#include "Direct2DWrapper.hpp" // 테스트용으로 넣었음
#include "FrameData.hpp"
#include "AnimationClip.hpp"

// [!!!주의사항!!!]
// 1. JSON 경로 및 파일 이름은 가급적 영문/ASCII로 작성할 것 (한글 경로는 로딩 실패 가능성 있음)
// 2. JSON 내부의 "이름(name)" 키값은 전역에서 유일해야 함 (중복되면 덮어쓰기되어 일부 클립이 증발함)

class ResourceManager : public Singleton<ResourceManager> {
	friend class Singleton<ResourceManager>;
private:
	ID2D1HwndRenderTarget* renderTarget = nullptr;
	IWICImagingFactory* wicFactory = nullptr; // PNG, JPG등을 D2D에 맞게 변환해주는 팩토리임(IWICImagingFactory)

	//std::unordered_map<std::wstring, ID2D1Bitmap*> bitmapCache; //대충 캐쉬겠지? 읽어오고 저장해두는	
	std::unordered_map<std::wstring, std::weak_ptr<BitmapResource>> bitmapCache; //구조 변경됨, 비트맵을 랩핑한 클래스의 인스턴스를 weak_ptr로 가짐

	std::unordered_map<std::wstring, std::shared_ptr<SheetData>> sheetCache;// json파일 캐싱해두는거임

	// 거지같은 중첩 해시맵 // 경로 | <동작(이름) | 클립> // json파일별로 읽어와서 정리하는거임
	// !!주의!! json 이름 겹치는거 있으면 안됨 + 한 동작이 두개의 json에 나뉘어져 들어가면 안됨(중복이라, 키가 덮어씌어짐) 알아서 잘 처리할것
	std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::shared_ptr<AnimationClip>>> clipCache;

public:
	void Initialize(ID2D1HwndRenderTarget* rt, IWICImagingFactory* wic);
	//ID2D1Bitmap* LoadBitmapFromFile(const std::wstring& path); 이전 사용방식과 달라져서 폐기함

	std::shared_ptr<BitmapResource> LoadBitmapResource(const std::wstring& path);
	
	std::shared_ptr<SheetData> LoadSheetData(const std::wstring& jsonPath, const std::wstring& imgPath);

	const std::unordered_map<std::wstring, std::shared_ptr<AnimationClip>>& LoadClipSet(const std::wstring& jsonPath);
	//const & 반환형으로 쓰는건 거의 처음이네, 내부에 있는걸 밖에 그대로 공유한다는 의미인듯? 반환 매개 둘다 const & 쓰는건 신기하넹

	//========================================================================
	//void Shutdown(); //더이상 스스로 릴리즈 할 이유 없음, 비트맵 소유권이 BitmapResource에게 넘어감

	//테스트 코드, 사용금지!!!
	ID2D1Bitmap* LoadBitmapRaw(const std::wstring& path) {
		IWICImagingFactory* wicFactory = nullptr;
		IWICBitmapDecoder* decoder = nullptr;
		IWICBitmapFrameDecode* frame = nullptr;
		IWICFormatConverter* converter = nullptr;
		ID2D1Bitmap* bitmap = nullptr;

		auto& d2d = Direct2D::GetInstance();
		ID2D1RenderTarget* renderTarget = d2d.GetRenderTarget();

		// WIC 팩토리 만들기
		if (FAILED(CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wicFactory)))) {
			return nullptr;
		}

		// 파일 디코더
		if (FAILED(wicFactory->CreateDecoderFromFilename(
			path.c_str(),
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&decoder))) {
			wicFactory->Release();
			return nullptr;
		}

		// 첫 번째 프레임 가져오기
		if (FAILED(decoder->GetFrame(0, &frame))) {
			decoder->Release();
			wicFactory->Release();
			return nullptr;
		}

		// 32비트 RGBA 포맷으로 변환
		if (FAILED(wicFactory->CreateFormatConverter(&converter))) {
			frame->Release();
			decoder->Release();
			wicFactory->Release();
			return nullptr;
		}

		if (FAILED(converter->Initialize(
			frame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.0,
			WICBitmapPaletteTypeCustom))) {
			converter->Release();
			frame->Release();
			decoder->Release();
			wicFactory->Release();
			return nullptr;
		}

		// D2D 비트맵으로 변환
		HRESULT hr = renderTarget->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);

		// 정리
		converter->Release();
		frame->Release();
		decoder->Release();
		wicFactory->Release();

		if (FAILED(hr)) return nullptr;
		return bitmap;
	}

};