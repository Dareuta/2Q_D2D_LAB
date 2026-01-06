//========================================================================
// ResourceManager.cpp
//========================================================================

#include "pch.h"
#include "ResourceManager.hpp"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

void ResourceManager::Initialize(ID2D1HwndRenderTarget* rt, IWICImagingFactory* wic) {
	renderTarget = rt;
	wicFactory = wic;
}

//========================================================================
// BitMap Load ///////////////////////////////////////////////////////////
//========================================================================

std::shared_ptr<BitmapResource> ResourceManager::LoadBitmapResource(const std::wstring& path) {
	//========================================================================
	// 기존에 있는지 먼저 캐시 확인함

	auto it = bitmapCache.find(path); // 이터레이터에 path 먹임
	if (it != bitmapCache.end()) { //find 했는데 있네?
		if (auto shared = it->second.lock()) //승격하는거임, 성공하면 true + 내부 객체가 살아있는지도 판단 됨, 실패시 nullptr
			return shared; // 이미 로드된걸 반환해줌
	}

	//========================================================================
	// 여기부터 진짜 진짜임

	IWICBitmapDecoder* decoder = nullptr;
	IWICBitmapFrameDecode* frame = nullptr;
	IWICFormatConverter* converter = nullptr;
	ID2D1Bitmap* bitmap = nullptr;

	//========================================================================
	// 이미지 디코더 생성 시도

	HRESULT hr = wicFactory->CreateDecoderFromFilename( // hr에다가 계속 뭔갈 시도하면서, 제대로 됐는지 검증하는 구조임
		path.c_str(), // 이미지 경로를 c스타일로 변환한거임 (경로) // LPCWSTR이 const wchar_t* 타입임 
		nullptr, // 아무튼 nullptr인데 까먹음 // 아무튼 일반적인 상황에서는 nullptr, 특정 벤더의 디코더를 지정한다는데 어디다 쓰는지 모르겟슴 그냥 nullptr임
		GENERIC_READ, // 읽기 전용일껄 
		WICDecodeMetadataCacheOnLoad, //로드할때, 메타데이터까지 전부 캐싱하는거, 아무튼 안정적이고 빠르다고 함 
		// 메타데이터는, 이미지의 부가 정보임(구조, 몇개, 프레임, 색공간 등을 담은 설명 데이터라고함)
		&decoder // 여기다 담아주세요 IWICBitmapDecoder임
	);

	//========================================================================
	if (FAILED(hr)) return nullptr; //////////////////////////////////////////
	//========================================================================

	hr = decoder->GetFrame(0, &frame); // 디코더로 찝은거의 첫번째 프레임을 반환해서 넣음 // 아마 WIC뭐시기가 gif도 지원해서 그럴껄?

	//========================================================================
	if (FAILED(hr)) goto CLEANUP; ////////////////////////////////////////////
	//========================================================================

	hr = wicFactory->CreateFormatConverter(&converter); //팩토리에서 컨버터 만들엉 
	// 중간처리기임, Direct2D가 선호하는 포맷: GUID_WICPixelFormat32bppPBGRA (premultiplied alpha 포함된 32비트 BGRA) 라고 함

	//========================================================================
	if (FAILED(hr)) goto CLEANUP; ////////////////////////////////////////////
	//========================================================================

	hr = converter->Initialize( // 아까 팩토리에서 주문한 컨버터 초기설정 해주는거임 < 즉, 포맷 설정임
		frame, //아까 찝은 첫 프레임 기준으로 (원본 이미지임)
		GUID_WICPixelFormat32bppPBGRA, // D2D가 좋아하는 포맷임, 그냥 이거 좋아하더라
		WICBitmapDitherTypeNone, // 뭔진 모르겠는데 아무튼 dithertype 이라는게 none인거임 
		// 디더링(dithering): 제한된 색상 수로 더 부드럽게 보이게 하려고 픽셀을 교묘하게 분산시키는 기술이라고 함
		nullptr, // 또나왔다 nullptr! 대충 기본이겠지, indexed color 이미지(JPEG, PNG 등 대부분은 팔레트 안 씀) 라고함, 일반적으로 nullptr
		0.0f, // 아마 알파값이었는데
		WICBitmapPaletteTypeMedianCut // 뭔진 모르겠는데 아무튼 팔레트타입이 메디안컷이라는 뜻임
									  // MedianCut: 이미지 컬러를 분석해서 팔레트 256색 이하로 압축할 때 쓰는 알고리즘이라고 함 
									  // 팔레트가 nullptr이라 사실 의미없지만, 일단 넣긴 해야해서 기본적으로 넣어줌
	);

	//========================================================================
	if (FAILED(hr)) goto CLEANUP; ////////////////////////////////////////////
	//========================================================================

	hr = renderTarget->CreateBitmapFromWicBitmap(converter, &bitmap); // 아까 정한 포맷(컨버터 - 안에 원본 이미지 들어있음)를 기반으로 bitmap 만들어줭

	//========================================================================
	if (FAILED(hr)) goto CLEANUP; ////////////////////////////////////////////
	//========================================================================
	// 랜더링 파이프라인은 끝났지만, 아직 우린 스마트 포인터를 관리 해야함


	{ //블록을 만들면, 블록 안에서만 스마트포인터의 수명이 어쩌구됨, 아무튼 영역 제한임
		auto resource = std::make_shared<BitmapResource>(bitmap); // 아까 만든 객체(bitmap)을, shared_ptr로 감쌈
		bitmap = nullptr; // 이거 해주는게 안전하다고 함

		bitmapCache[path] = resource; //언오더맵에 shared_ptr을 담음
		//엥? 왜 weak_ptr이 아니라 shared_ptr을 담는거지? 우째서...?
		//shared <-> waek 암시적 형변환이 가능함. 그러므로 가능임

		return resource; // 반환해줌, 바로 사용하라고 + nullptr비교
	}

	// 이 아래는 안가는게 정상임
CLEANUP: // 라벨, 절 대 사 용 하 지 말 라 고 했 지 만, 쓸 꺼 지 롱
	// 더러운거 전부 릴리즈해서 정리함
	// 생성의 역순으로 해제하는게 더 안전하다네, 아, 조립은 분해의 역순!
	if (bitmap) bitmap->Release();
	if (converter) converter->Release();
	if (frame) frame->Release();
	if (decoder) decoder->Release();

	return nullptr; //여길 왔다고? 이곳에 빛은 없다 포기해라 인간, nullptr
}

std::wstring ToWString(const std::string& str) { // 유틸 함수, string > wstring 변환해주는거임
	return std::wstring(str.begin(), str.end()); // 주의!!! 이거 한글 깨진다. 경로는 영어로 유지해야함!
}

//========================================================================
// SpriteSheet Load //////////////////////////////////////////////////////
//========================================================================

std::shared_ptr<SheetData> ResourceManager::LoadSheetData(const std::wstring& jsonPath, const std::wstring& imgPath) {

	auto it = sheetCache.find(jsonPath);
	if (it != sheetCache.end()) return it->second; // 캐시 확인 후, 있으면 바로 반환해줌

	// 이미지 로드 시도
	auto bitmap = LoadBitmapResource(imgPath); // 아틀라스 읽어오기
	if (!bitmap) throw std::runtime_error("FAILED TO LOAD BITMAP");;

	// Json 읽기
	std::ifstream file(jsonPath);
	if (!file.is_open()) throw std::runtime_error("FAILED TO OPEN JSON(sheet)");

	json j;
	file >> j; // 엄청 간단하지만, 아무튼 읽는거임

	//본격적으로 등록할 인스턴스 만듬
	auto sheet = std::make_shared<SheetData>();
	sheet->bitmap = bitmap; // 비트맵(아틀라스) 저장함

	// Json은 내부적으로 배열 / 맵처럼 작동함
	// 그래서, 포문에다가 [name, data]를 넣는건 사실
	// 배열의 첫번째항, 두번쨰항을 각 변수에 담아달라는 의미임
	// ["여기는"] 키 값이 들어감, 즉 저 문자가 있는곳을 찾아서
	// 첫번째, 두번째 항을 찾아서 집어넣는 구조임
	// 첫번째 항은 스스로가 키로 작동함!

	//어쩔 수 없이 17버전으로 올렸음, 엄청 편하네
	for (auto& [name, data] : j["frames"].items()) {
		FrameData frame;
		frame.name = ToWString(name);

		const auto& rect = data["frame"];
		int x = rect["x"];
		int y = rect["y"];
		int w = rect["w"];
		int h = rect["h"];

		frame.rect = {
			static_cast<float>(x),
			static_cast<float>(y),
			static_cast<float>(x + w),
			static_cast<float>(y + h)
		};

		const auto& pivot = data["pivot"];
		frame.pivot.x = pivot.value("x", 0.5f);
		frame.pivot.y = pivot.value("y", 0.5f);

		sheet->frames[frame.name] = frame;
	}
	sheetCache[jsonPath] = sheet; // 캐시 저장
	return sheet; // 바로 쓰라고 돌려줌
}

//========================================================================
// AnimationClip Load ////////////////////////////////////////////////////
//========================================================================

const std::unordered_map<std::wstring, std::shared_ptr<AnimationClip>>& ResourceManager::LoadClipSet(const std::wstring& jsonPath) {
	auto it = clipCache.find(jsonPath);
	if (it != clipCache.end()) return it->second; // 캐시 확인 후, 있으면 돌려줌

	std::ifstream file(jsonPath);
	if (!file.is_open()) throw std::runtime_error("FAILED TO OPEN JSON(clip)");

	json j;
	file >> j;

	auto& outMap = clipCache[jsonPath];

	for (auto& [clipName, clipData] : j.items()) {
		auto clip = std::make_shared<AnimationClip>();
		clip->clipName = ToWString(clipName);

		clip->looping = clipData.value("looping", false);

		if (!clipData.contains("frames") || !clipData["frames"].is_array()) continue;

		for (auto& frameData : clipData["frames"]) {
			AnimationFrame f;
			f.spriteName = ToWString(frameData["name"]);
			f.duration = frameData.value("duration", 0.1f);
			clip->frames.push_back(f);
		}
		outMap[clip->clipName] = clip; // 클립 스스로의 이름으로 넣는거임 클립은 배열을 소유함
	}

	return outMap;
}

//========================================================================

/* 비트맵리소스 클래스로 랩핑하여 사용해서 폐기함
ID2D1Bitmap* ResourceManager::LoadBitmapFromFile(const std::wstring& path) {
	// 이미 로드되어있는지 확인
	auto it = bitmapCache.find(path); // 경로(키)를 가진게 있는지 찾음
	if (it != bitmapCache.end()) { return it->second; } // 캐싱해둔거 바로 반환시킴

	//========================================================================

	IWICBitmapDecoder* decoder = nullptr; // 이미지를 읽어옴, 추출하는 등
	IWICBitmapFrameDecode* frame = nullptr; // 원본 이미지
	IWICFormatConverter* converter = nullptr; // D2D에서 요구하는 양식으로 바꿔줌
	ID2D1Bitmap* bitmap = nullptr; // 완성본

	//========================================================================
	// 디코더 생성
	HRESULT hr = wicFactory->CreateDecoderFromFilename(
		path.c_str(),					// [1] 이미지 파일 경로 (std::wstring → const wchar_t*)
		nullptr,						// [2] GUID: 특정 디코더 지정 (보통 nullptr로 자동 감지)
		GENERIC_READ,					// [3] 접근 권한: 읽기 전용
		WICDecodeMetadataCacheOnLoad,	// [4] 메타데이터 로딩 방식 (전부 미리 로딩)
		&decoder						// [5] 결과 포인터 (out param): IWICBitmapDecoder** 타입
	);

	//========================================================================
	if (FAILED(hr)) return nullptr; //////////////////////////////////////////
	//========================================================================

	// 첫 번째 프레임 가져오기 // WIC이미지는 여러장(GIF)로 구성될 수 있기 떄문에, 첫번째 항목을 로드함 // 내부는 프레임배열처럼 작동함
	hr = decoder->GetFrame(0, &frame);

	//========================================================================
	if (FAILED(hr)) goto CLEANUP; ////////////////////////////////////////////
	//========================================================================

	// 포맷 변환기 생성
	hr = wicFactory->CreateFormatConverter(&converter); // D2D가 요구하는 포맷으로 변환하기 위함

	//========================================================================
	if (FAILED(hr)) goto CLEANUP; ////////////////////////////////////////////
	//========================================================================

	// BGRA 변환 초기화(원본 프레임 + 원하는 포맷 지정)
	hr = converter->Initialize(
		frame,                                 // [1] 원본 이미지 프레임 (디코더에서 얻은 IWICBitmapFrameDecode*)
		GUID_WICPixelFormat32bppPBGRA,         // [2] 원하는 출력 포맷 (Direct2D가 요구하는 32bit BGRA + 알파 사전 곱)
		WICBitmapDitherTypeNone,               // [3] 디더링 없음 (색수 줄일 필요 없음, 부드러운 그라데이션 손실 방지)
		nullptr,                               // [4] 사용자 정의 팔레트 없음 (indexed color가 아니라면 항상 nullptr)
		0.0f,                                  // [5] 알파 임계값 (팔레트용. 여기선 의미 없음)
		WICBitmapPaletteTypeMedianCut          // [6] 팔레트 추출 방식 (팔레트 사용 안 하므로 의미 없음, but 반드시 지정 필요)
	);

	//========================================================================
	if (FAILED(hr)) goto CLEANUP; ////////////////////////////////////////////
	//========================================================================

	// 비트맵 생성
	hr = renderTarget->CreateBitmapFromWicBitmap(converter, &bitmap);

	//========================================================================
	if (FAILED(hr)) goto CLEANUP; ////////////////////////////////////////////
	//========================================================================

	bitmapCache[path] = bitmap; //비트맵 캐시에 저장

	//========================================================================
CLEANUP: //라벨언어, goto로 넘어오거나, 마지막에 실행됨
	if (decoder) decoder->Release();
	if (frame) frame->Release();
	if (converter) converter->Release();

	return bitmap;
}
*/

/*
void ResourceManager::Shutdown() {
	for (std::pair<const std::wstring, ID2D1Bitmap*>& pair : bitmapCache) { //내부적으로 unordered_map key << const임
		if (pair.second) { pair.second->Release(); } // com객체 릴리즈
	}
	bitmapCache.clear();
}
*/