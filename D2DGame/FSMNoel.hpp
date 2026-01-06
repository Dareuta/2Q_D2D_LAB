//========================================================================
// FSMNoel.hpp
//========================================================================

#pragma once

#include "IFSM.hpp"
#include "AnimationAssets.hpp"
#include "AnimationComponent.hpp"
#include "ResourceManager.hpp"
#include <unordered_map>

class FSMNoel : public IFSM {
public:
	//========================================================================
	// 논리를 따질때 사용하는 상태들
	//========================================================================

	enum BasicState { // IDLE MOVE JUMP ATTACK 크게 4종류
		None = 0,
		Idle, // 추가 대기모션 고려
		Move, // 달리기 + 걷기 + 방향전환
		Jump, // 착지 + 점프
		Attack // 공격 1, 2 추가가능
	};

	//========================================================================
	// 애니메이션 출력을 위한, 클립과 1대1 대응하는 상태들
	//========================================================================
	// 여기 enum은, 대문자 앞에 붙여서 구분하면 편함

	enum IdleState {
		I_None,
		I_Idle,
		I_Special // 특수 모션 - 도발모션 같은거
	};

	enum MoveState {
		M_None,
		M_WalkF, // 앞으로 걷기
		M_WalkB, // 뒤로 걷기
		M_trun, // 방향전환 - 걷기일때만
		M_Run // 달리기 ( 멈추는 프레임 포함 )		
	};

	enum JumpState {
		J_None,
		J_Jump, // 제자리 점프
		J_JumpF, // 앞점프
		J_JumpB, // 뒷점프
		J_Landing, // 제자리 착지
		J_LandingFB // 방향성 착지
	};

	enum AttackState {
		A_None,
		A_Attack1,
		A_Attack2
	};

	bool isGrounded = false; // 테스트용 임시 플래그 << 나중에 물리엔진에서 추가해줘야함
	bool xFlip = false; // 뒤집는 임시 플래그 << 마찬가지로 나중에 옮겨줘야함
	//========================================================================
private:
	struct State { // 상태들 묶음
		BasicState basic = None;
		AttackState attack = A_None;
		JumpState jump = J_None;
		MoveState move = M_None;
		IdleState idle = I_None;
	};

	// 문자 - 상태(논리) 연결임
	std::unordered_map<std::wstring, BasicState> stateTable = {
		{L"Attack", BasicState::Attack},
		{L"Move",	BasicState::Move},
		{L"Idle",	BasicState::Idle},
		{L"Jump",	BasicState::Jump},
	};

	std::unordered_map<BasicState, std::wstring> stringTable = {
		{BasicState::Attack,L"Attack"},
		{BasicState::Move,	L"Move"},
		{BasicState::Idle,	L"Idle"},
		{BasicState::Jump,	L"Jump"},
	};

	State state;
	GameObject* object = nullptr; // 유니크ptr에서 로우포인터를 받아오는거임, 수정하면 안됨

public:
	//========================================================================	
	// 인터페이스 함수
	//========================================================================
	void Enter() override {

	}

	void Exit() override {

	}

	void Update(float delta, InputState input) override { // 인풋은 외부에 공개된 변수임 밖에서 넣어주면 바로 읽어오는 구조
		switch (state.basic) {
		case(Attack):	DoAttack(input);	break;
		case(Jump):		DoJump(input);		break;
		case(Move):		DoMove(input);		break;
		case(Idle):		DoIdle(input);		break;
		default: break;
		}
	}

	bool TrySetState(const std::wstring& next) override {
		auto it = stateTable.find(next);
		if (it != stateTable.end()) {
			BasicState value = it->second;
			if (CheckTransition(value)) { // 유효성 검사 통과
				state.basic = value; // 논리 변경, 다음 업데이트에서 input에 따라 변경됨				
				return true;
			}
		}

		return false;
	}

	bool TrySetState(BasicState next) {
		if (CheckTransition(next)) {
			state.basic = next;
			return true;
		}
		return false;
	}

	void RegistOwner(GameObject* owner) override {
		if (owner) {
			object = owner;
			Initialize();
		}
	}

	void Init() override {
		Initialize();
	}

private:
	//========================================================================
	// FSM 내부 함수 (내부 로직용)
	//========================================================================

	void DoAttack(InputState input) { // 공격의 경우, 프레임이 끝나면 Idle로 복귀함	
		// 나중에 처리할꺼 있으면 여기 넣으면 됨
	}

	// 걷는건 유지해야 지속됨, 즉. 때면 멈춰야하고, 다른방향을 누르면 바로 바뀌어야함
	void DoMove(InputState input) { // 기본적으로 캐릭터는 왼쪽을 바라봄, xFlip = false << 기본값				
		switch (input) {
		case InputState::KeyA:
			if (TrySetState(Attack)) {
				state.attack = A_Attack1;
				ApplyAnimation(NoelClip::Attack1, Noel::Attack.json, Noel::Attack.png);
			}
			break;

		case InputState::KeyB:
			if (TrySetState(Attack)) {
				state.attack = A_Attack2;
				ApplyAnimation(NoelClip::Attack2, Noel::Attack.json, Noel::Attack.png);
			}
			break;

		case InputState::Right:
			state.move = M_WalkB;
			ApplyAnimation(NoelClip::WalkB, Noel::Move.json, Noel::Move.png);
			break;

		case InputState::Left:
			state.move = M_WalkF;
			ApplyAnimation(NoelClip::WalkF, Noel::Move.json, Noel::Move.png);

			break;

		case InputState::RightUp:
			if (TrySetState(Jump)) {
				state.jump = J_JumpB;
				isGrounded = false;
				ApplyAnimation(NoelClip::JumpB, Noel::Jump.json, Noel::Jump.png);
			}
			break;

		case InputState::LeftUp:
			if (TrySetState(Jump)) {
				state.jump = J_JumpF;
				isGrounded = false;
				ApplyAnimation(NoelClip::JumpF, Noel::Jump.json, Noel::Jump.png);
			}
			break;

		case InputState::Up:
			if (TrySetState(Jump)) {
				state.jump = J_Jump;
				isGrounded = false;
				ApplyAnimation(NoelClip::Jump, Noel::Jump.json, Noel::Jump.png);
			}
			break;

		case InputState::Down:
			isGrounded = true;
			break;

		default:
			if (TrySetState(Idle)) {
				state.move = M_None;
				ApplyAnimation(NoelClip::Idle, Noel::Idle.json, Noel::Idle.png);
			}
			break;
		}
	}

	void DoJump(InputState input) {

		if (input == InputState::Down) isGrounded = true;
	}

	void DoIdle(InputState input) {
		switch (input) {
		case InputState::KeyA:
			if (TrySetState(Attack)) {
				state.attack = A_Attack1;
				ApplyAnimation(NoelClip::Attack1, Noel::Attack.json, Noel::Attack.png);
			}
			break;

		case InputState::KeyB:
			if (TrySetState(Attack)) {
				state.attack = A_Attack2;
				ApplyAnimation(NoelClip::Attack2, Noel::Attack.json, Noel::Attack.png);
			}
			break;

		case InputState::Right:
			if (TrySetState(Move)) {
				state.move = M_WalkB;
				ApplyAnimation(NoelClip::WalkB, Noel::Move.json, Noel::Move.png);
			}
			break;

		case InputState::Left:
			if (TrySetState(Move)) {
				state.move = M_WalkF;
				ApplyAnimation(NoelClip::WalkF, Noel::Move.json, Noel::Move.png);
			}
			break;

		case InputState::RightUp:
			if (TrySetState(Jump)) {
				state.jump = J_JumpB;
				isGrounded = false;
				ApplyAnimation(NoelClip::JumpB, Noel::Jump.json, Noel::Jump.png);
			}
			break;

		case InputState::LeftUp:
			if (TrySetState(Jump)) {
				state.jump = J_JumpF;
				isGrounded = false;
				ApplyAnimation(NoelClip::JumpF, Noel::Jump.json, Noel::Jump.png);
			}
			break;

		case InputState::Up:
			if (TrySetState(Jump)) {
				state.jump = J_Jump;
				isGrounded = false;
				ApplyAnimation(NoelClip::Jump, Noel::Jump.json, Noel::Jump.png);
			}
			break;

		case InputState::Down:
			isGrounded = true;
			break;

		default:
			break;
		}
	}

	// 임시로 전부 true로 바꿈, 나중에 변경해야함!
	bool CheckTransition(BasicState next) { // 상태 전이 유효성 검사
		if (next == state.basic) return false; // 같은 상태전이는 허락하지 않음

		switch (next) {
		case(Attack): return true; // 전부 불가능함 Idle복귀는 알아서 내부에서 처리
		case(Move): return true; // 생각해보니 이것도 전부 가능함			
		case(Jump): return true; // 점프는 공중이동이라 착지할때까지 전이 불가			
		case(Idle): return true; // 전부가능함
		default: break;
		}

		return false;
	}

	void ApplyAnimation(const std::wstring& clipName, const std::wstring& sheetJson, const std::wstring& sheetPng) {
		if (!object) return;

		auto fr = object->GameObject::GetComponent<AnimationStateComponent>();
		auto cl = object->GameObject::GetComponent<AnimationClipComponent>();
		if (!fr || !cl) return;

		fr->SetClip(clipName);
		cl->sheetData = ResourceManager::GetInstance().LoadSheetData(sheetJson, sheetPng);
	}

	//========================================================================
	// 콜백 등록 // 애니메이션 끝났을때 처리를 어떻게할지에 대해 기록함
	//========================================================================

	void Initialize() {
		auto fr = object->GameObject::GetComponent<AnimationStateComponent>();
		auto cl = object->GameObject::GetComponent<AnimationClipComponent>();
		if (!fr || !cl) return;

		fr->currentFrame = 0;
		fr->timer = 0;
		fr->isFinished = false;
		
		state.basic = Idle;		
		ApplyAnimation(NoelClip::Idle, Noel::Idle.json, Noel::Idle.png);

		fr->onClipFinished = [this]() {
			auto fr1 = object->GameObject::GetComponent<AnimationStateComponent>();
			auto cl1 = object->GameObject::GetComponent<AnimationClipComponent>();

			if (state.basic == Attack) { // 끝나면 원래 IDLE 복귀하면 됨
				state.attack = A_None;
				state.basic = Idle;
				ApplyAnimation(NoelClip::Idle, Noel::Idle.json, Noel::Idle.png);
			}
			else if (state.basic == Jump) {
				switch (state.jump) {
				case J_JumpF:
				case J_JumpB: //점프 애니메이션이 끝났을때, 지상이라면 착지, 아니라면 반복
					if (isGrounded) { // 지상						
						state.jump = J_LandingFB;
						ApplyAnimation(NoelClip::LandingFB, Noel::Jump.json, Noel::Jump.png); // 착지 시작
					}
					else {
						fr1->currentFrame -= 1;
						fr1->timer = 0.0f;
						fr1->isFinished = false;
					}
					break;

				case J_Jump:
					if (isGrounded) { // 지상						
						state.jump = J_Landing;
						ApplyAnimation(NoelClip::Landing, Noel::Jump.json, Noel::Jump.png); // 착지 시작
					}
					else {
						fr1->currentFrame -= 1;
						fr1->timer = 0.0f;
						fr1->isFinished = false;
					}
					break;
				case J_Landing:
				case J_LandingFB:
					state.jump = J_None;
					state.basic = Idle;
					ApplyAnimation(NoelClip::Idle, Noel::Idle.json, Noel::Idle.png);
					break;
				default: break;
				}
			}

			};

	}
};