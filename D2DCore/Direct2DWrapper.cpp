//========================================================================
// Direct2DWrapper.cpp
//========================================================================

#include "pch.h"
#include "Direct2DWrapper.hpp"

bool Direct2D::Initialize(HWND hWnd) {
	HRESULT hr; // Windows API에서 오류 처리 및 상태 반환에 사용하는 32비트 값의 정수형
	// 성공 or 실패 여부 + 추가정보가 담기는 변수

	// D2D1 팩토리 생성
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory); //싱글 스레드
	if (FAILED(hr)) return false;

	// 윈도우 크기 얻기
	RECT rc;
	GetClientRect(hWnd, &rc);

	// 랜더 타겟 생성 D2D1_SIZE_U << 양수값 2d 크기 구조체
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	hr = pFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(), // 렌더 타겟 일반 속성
		D2D1::HwndRenderTargetProperties(hWnd, size),// HWND 기반의 타겟 속성
		&pRenderTarget);			// 결과 포인터 // out parameter 패턴

	if (FAILED(hr)) return false;

	//WIC 이미지 팩토리 생성 (PNG전용)
	hr = CoInitialize(nullptr); // COM 초기화 - 한번만
	if (FAILED(hr)) return false;

	hr = CoCreateInstance(
		CLSID_WICImagingFactory,	// 생성할 COM 객체의 클래스 ID
		nullptr,					// 집어넣을 기존 객체 없음 (새로 생성)
		CLSCTX_INPROC_SERVER,		// DLL 안에서만 동작 (프로세스 내부) // 대부분 이거 씀
		IID_PPV_ARGS(&pWICFactory)  // 반환받을 인터페이스 포인터 (out param)
		//IWICImagingFactory 인터페이스를 내놔, 이 포인터에 담으라는 뜻
	);
	if (FAILED(hr)) return false;

	hr = DWriteCreateFactory(		//팩토리 만들장
		DWRITE_FACTORY_TYPE_SHARED, //공유 팩토리. 시스템 전체에서 공용으로 사용되는 설정(폰트, 캐시같은거) 공유

		__uuidof(IDWriteFactory), // 이게 뭔 거지같은거냐, GUID를 컴파일 타임에서 얻어내는 키워드임
		// 만약 하드 코딩한다면 {B859EE5A-D838-4B5B-A2F4-7D6F4D6E5CA8} 같은 끔찍한걸 넣어야함
		// GUID? COM 인터페이스의 고유 ID임

		reinterpret_cast<IUnknown**>(&dwriteFactory) //IDWriteFactory << IUnknown이 부모임
		//안에서 내용물 담아달라고 건네주는거라 **임, 우리가 필요한건 *라서
	);
	if (FAILED(hr)) return false;

	return true;

	//포인터로 리턴하는 이유는, 반환값이 하나로 고정되어있는걸 해결하기 위함 +
	//성공 실패 여부를 반환시켜서 관리하기 편하게 하기 위해서 인듯?
	//HRESULT << 사실상 bool + 실패한 이유담는 형태
}

/* [ResourceManager 에서 구현해서 삭제]

// 이미지 파일 로딩
// 지정된 파일 경로에서 PNG/JPG 등의 이미지를 로드하고
// Direct2D용 ID2D1Bitmap으로 변환하여 반환하는 함수

ID2D1Bitmap* Direct2D::LoadBitmapFromFile(const wchar_t* filename) {
	// WIC(Windows Imaging Component)에서 사용하는 디코더/프레임/변환기 포인터 선언
	IWICBitmapDecoder* pDecoder = nullptr;        // 이미지 파일 전체를 디코딩할 디코더
	IWICBitmapFrameDecode* pFrame = nullptr;      // 디코더가 가진 프레임 중 하나 (보통 첫 번째)
	IWICFormatConverter* pConverter = nullptr;    // 픽셀 포맷 변환을 위한 컨버터
	ID2D1Bitmap* pBitmap = nullptr;               // 최종적으로 Direct2D에서 사용할 비트맵 결과

	// -----------------------------------------
	// [1] 이미지 파일 경로에서 디코더 생성
	// - 파일 경로: filename
	// - GENERIC_READ: 읽기 전용
	// - WICDecodeMetadataCacheOnLoad: 전체 메타데이터를 즉시 로드
	// - &pDecoder: 디코더 인터페이스를 이 포인터에 넣어달라는 요청
	HRESULT hr = pWICFactory->CreateDecoderFromFilename(
		filename, nullptr, GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	// 만약 디코더 생성에 실패했다면, 리소스도 없으니 그냥 바로 nullptr 반환
	if (FAILED(hr)) return nullptr;

	// -----------------------------------------
	// [2] 첫 번째 프레임을 가져옴 (프레임 = 이미지의 실질적인 픽셀 데이터)
	// - 일반적인 이미지 파일은 프레임이 1개
	// - GIF나 TIFF는 여러 프레임 가질 수 있음
	hr = pDecoder->GetFrame(0, &pFrame);
	if (FAILED(hr)) goto CLEANUP;

	// -----------------------------------------
	// [3] 픽셀 포맷 변환기 생성
	// - Direct2D가 지원하는 포맷(PBGRA)로 변환하기 위해 필요
	hr = pWICFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr)) goto CLEANUP;

	// -----------------------------------------
	// [4] 변환기 초기화
	// - 원본: pFrame
	// - 변환 포맷: 32bit Premultiplied BGRA (Direct2D가 최적으로 사용하는 포맷)
	// - 디더링 없음, 팔레트 없음, 알파 0.0f (기본값)
	hr = pConverter->Initialize(
		pFrame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0f,
		WICBitmapPaletteTypeMedianCut
	);
	if (FAILED(hr)) goto CLEANUP;

	// -----------------------------------------
	// [5] 변환된 WIC 비트맵을 Direct2D용 ID2D1Bitmap으로 생성
	// - 최종적으로 Direct2D가 그릴 수 있는 비트맵 객체가 만들어짐
	hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, &pBitmap);
	if (FAILED(hr)) goto CLEANUP;

	// -----------------------------------------
	// [CLEANUP] 중간 실패 시 리소스를 안전하게 해제
CLEANUP:
	// COM 객체는 Release() 해줘야 누수 방지됨
	if (pDecoder) pDecoder->Release();       // 디코더 해제
	if (pFrame) pFrame->Release();           // 프레임 해제
	if (pConverter) pConverter->Release();   // 컨버터 해제

	// 최종적으로 성공한 경우 pBitmap이 nullptr이 아님
	return pBitmap;
}
*/

void Direct2D::BeginDraw() {
	if (pRenderTarget) pRenderTarget->BeginDraw();
}

void Direct2D::EndDraw() {
	if (pRenderTarget) pRenderTarget->EndDraw();
}

void Direct2D::Clear(D2D1::ColorF color) {
	if (pRenderTarget) pRenderTarget->Clear(color);
}