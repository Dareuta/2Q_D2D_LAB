//========================================================================
// AnimationAssets.hpp
//========================================================================

#pragma once

#include <string>

struct AnimationAsset {
	std::wstring json;
	std::wstring png;
};

namespace Noel {
	inline constexpr const wchar_t* ClipSet = L"assets/Noel/Noel_CombinedClip.json";
	inline const AnimationAsset Idle = { L"assets/Noel/Noel_Idle.json",   L"assets/Noel/Noel_Idle.png" };
	inline const AnimationAsset Attack = { L"assets/Noel/Noel_Attack.json", L"assets/Noel/Noel_Attack.png" };
	inline const AnimationAsset Change = { L"assets/Noel/Noel_Change.json", L"assets/Noel/Noel_Change.png" };
	inline const AnimationAsset Effect = { L"assets/Noel/Noel_Effect.json", L"assets/Noel/Noel_Effect.png" };
	inline const AnimationAsset Jump = { L"assets/Noel/Noel_Jump.json", L"assets/Noel/Noel_Jump.png" };
	inline const AnimationAsset Move = { L"assets/Noel/Noel_Move.json", L"assets/Noel/Noel_Move.png" };
	inline const AnimationAsset Special = { L"assets/Noel/Noel_Special.json", L"assets/Noel/Noel_Special.png" };
}

namespace NoelClip {
	inline constexpr const wchar_t* Idle = L"no000";
	inline constexpr const wchar_t* Attack1 = L"no330";	
	inline constexpr const wchar_t* Turn = L"no003";  
	inline constexpr const wchar_t* Jump = L"no020";
	inline constexpr const wchar_t* JumpF = L"no021";  
	inline constexpr const wchar_t* JumpB = L"no022";  
	inline constexpr const wchar_t* Landing = L"no023";
	inline constexpr const wchar_t* LandingFB = L"no024";
	inline constexpr const wchar_t* WalkF = L"no030"; 
	inline constexpr const wchar_t* WalkB = L"no031"; 
	inline constexpr const wchar_t* Run = L"no032";  
	inline constexpr const wchar_t* Attack2 = L"no202";  
	inline constexpr const wchar_t* Special = L"no605"; 
	inline constexpr const wchar_t* Effect = L"vrnoef"; 
}

