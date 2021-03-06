// Fill out your copyright notice in the Description page of Project Settings.

#include "IreneFSM.h"
#include "IreneCharacter.h"
#include "IreneInputInstance.h"
#include "IreneUIManager.h"
#include "IreneAnimInstance.h"
#include "IreneAttackInstance.h"

#pragma region IreneFSM
void UIreneFSM::Update(const float Value)
{
	this->PlayTime += Value;
	StateValue->Execute(this);
}
void UIreneFSM::ChangeState(IState* NewState)
{
	StateValue->Exit(this);
	StateValue = NewState;
	StateValue->Enter(this);
}
void UIreneFSM::ThrowState(IState* NewState)
{
	NewState->Enter(this);
	NewState->Exit(this);
}
void UIreneFSM::Init(AIreneCharacter* Value)
{
	SetIreneCharacter(Value);
}
void UIreneFSM::SetIreneCharacter(AIreneCharacter* Value)
{
	Irene = Value;
}
FString UIreneFSM::GetStateToString() const
{
	switch (StateEnumValue)
	{
	case EStateEnum::Idle: return FString("Idle");
	case EStateEnum::BattleIdle: return FString("BattleIdle");
	case EStateEnum::Run_Loop: return FString("Run_Loop");
	case EStateEnum::Run_End: return FString("Run_End");
	case EStateEnum::Sprint_Loop: return FString("Sprint_Loop");
	case EStateEnum::Sprint_End: return FString("Sprint_End");
	case EStateEnum::Dodge_F_Start: return FString("Dodge_F_Start");
	case EStateEnum::Dodge_F_End: return FString("Dodge_F_End");
	case EStateEnum::Dodge_W_Start: return FString("Dodge_W_Start");
	case EStateEnum::Dodge_W_End: return FString("Dodge_W_End");
	case EStateEnum::Dodge_T_Start: return FString("Dodge_T_Start");
	case EStateEnum::Dodge_T_End: return FString("Dodge_T_End");
	case EStateEnum::Jump_Start: return FString("Jump_Start");
	case EStateEnum::Jump_Loop: return FString("Jump_Loop");
	case EStateEnum::Jump_End: return FString("Jump_End");
	case EStateEnum::B_Attack_1_F: return FString("B_Attack_1_F");
	case EStateEnum::B_Attack_2_F: return FString("B_Attack_2_F");
	case EStateEnum::B_Attack_3_F: return FString("B_Attack_3_F");
	case EStateEnum::B_Attack_1_W: return FString("B_Attack_1_W");
	case EStateEnum::B_Attack_2_W: return FString("B_Attack_2_W");
	case EStateEnum::B_Attack_3_W: return FString("B_Attack_3_W");
	case EStateEnum::B_Attack_1_T: return FString("B_Attack_1_T");
	case EStateEnum::B_Attack_2_T: return FString("B_Attack_2_T");
	case EStateEnum::B_Attack_3_T: return FString("B_Attack_3_T");
	case EStateEnum::Skill_F_Start: return FString("Skill_F_Start");
	case EStateEnum::Skill_F_End: return FString("Skill_F_End");
	case EStateEnum::Skill_W_Start: return FString("Skill_W_Start");
	case EStateEnum::Skill_W_End: return FString("Skill_W_End");
	case EStateEnum::Skill_T_Start: return FString("Skill_T_Start");
	case EStateEnum::Skill_T_End: return FString("Skill_T_End");
	case EStateEnum::Charge_1: return FString("Charge_1");
	case EStateEnum::Charge_2: return FString("Charge_2");
	case EStateEnum::Form_Change: return FString("Form_Change");
	case EStateEnum::Hit_1: return FString("Hit_1");
	case EStateEnum::Hit_2: return FString("Hit_2");
	case EStateEnum::Death: return FString("Death");
	default: return FString("Error StateEnumToString");
	}
}
FName UIreneFSM::GetStateToAttackDataTableName() const
{
	switch (StateEnumValue)
	{
	case EStateEnum::B_Attack_1_F: return FName("B_Attack_1_F");
	case EStateEnum::B_Attack_2_F: return FName("B_Attack_2_F");
	case EStateEnum::B_Attack_3_F: return FName("B_Attack_3_F");
	case EStateEnum::B_Attack_1_W: return FName("B_Attack_1_W");
	case EStateEnum::B_Attack_2_W: return FName("B_Attack_2_W");
	case EStateEnum::B_Attack_3_W: return FName("B_Attack_3_W");
	case EStateEnum::B_Attack_1_T: return FName("B_Attack_1_T");
	case EStateEnum::B_Attack_2_T: return FName("B_Attack_2_T");
	case EStateEnum::B_Attack_3_T: return FName("B_Attack_3_T");
	case EStateEnum::Skill_F_Start: return FName("ActionKeyword_1_F");
	case EStateEnum::Skill_F_End: return FName("ActionKeyword_1_F");
	case EStateEnum::Skill_W_Start: return FName("ActionKeyword_1_W");
	case EStateEnum::Skill_W_End: return FName("ActionKeyword_1_W");
	case EStateEnum::Skill_T_Start: return FName("ActionKeyword_1_T");
	case EStateEnum::Skill_T_End: return FName("ActionKeyword_1_T");
	default: return FName("Error GetStateToAttackDataTableName");
	}
}
#pragma endregion IreneFSM

#pragma region IdleState
#pragma region UIdleState
UIdleState* UIdleState::GetInstance()
{
	static UIdleState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UIdleState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UIdleState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Idle);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	// Irene?? ?????? UI?????? ???? ????
	if(CurState->Irene && CurState->Irene->IreneUIManager)
		CurState->Irene->IreneUIManager->HPRecoveryWaitStart();
}

void UIdleState::Execute(IBaseGameEntity* CurState)
{
	
}

void UIdleState::Exit(IBaseGameEntity* CurState)
{
	if (CurState->Irene->HpRecoveryData.bIsRecovering == true)CurState->Irene->IreneUIManager->HpRecoveringCancel();
	else CurState->Irene->IreneUIManager->HPRecoveryWaitCancel();
	CurState->bIsEnd = true;
}
#pragma endregion IdleState
#pragma region UBattleIdleState
UBattleIdleState* UBattleIdleState::GetInstance()
{
	static UBattleIdleState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UBattleIdleState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UBattleIdleState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::BattleIdle);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void UBattleIdleState::Execute(IBaseGameEntity* CurState)
{
	
}

void UBattleIdleState::Exit(IBaseGameEntity* CurState)
{
	CurState->bIsEnd = true;
}
#pragma endregion UBattleIdleState
#pragma endregion IdleState

#pragma region RunState
#pragma region URunLoopState
URunLoopState* URunLoopState::GetInstance()
{
	static URunLoopState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<URunLoopState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void URunLoopState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Run_Loop);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->GetCharacterMovement()->MaxWalkSpeed = CurState->Irene->IreneData.RunMaxSpeed;
	CurState->Irene->Weapon->SetVisibility(false);
}

void URunLoopState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveForward();
	CurState->Irene->IreneInput->MoveRight();
	if(CurState->PlayTime >= 3)
	{
		CurState->ThrowState(URunEndState::GetInstance());
		CurState->Irene->ChangeStateAndLog(USprintLoopState::GetInstance());
	}
	const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
	if (MoveKey[0] == 0 && MoveKey[1] == 0 && MoveKey[2] == 0 && MoveKey[3] == 0)
	{
		// if (Irene->IreneState->GetStateToString().Compare(FString("Sprint")) == 0)
		// {
		// 	Irene->IreneAnim->SetSprintStopAnim(true);
		// 	FTimerHandle TimerHandle;
		// 	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		// 		{
		// 			Irene->IreneAnim->SetSprintStopAnim(false);
		// 		}, 0.3f, false);
		// }
		CurState->ThrowState(URunEndState::GetInstance());
		CurState->Irene->ChangeStateAndLog(UIdleState::GetInstance());
	}
	if(MoveKey[0] != 0 || MoveKey[2] != 0)
	{
		CurState->Irene->SetUseCameraLag(CurState->Irene->CameraLagCurve[0]);
	}
	if(MoveKey[1] != 0 || MoveKey[3] != 0)
	{
		CurState->Irene->SetUseCameraLag(CurState->Irene->CameraLagCurve[1]);
	}
	if((MoveKey[0] != 0 && MoveKey[1] != 0) || (MoveKey[0] != 0 && MoveKey[3] != 0)||
		(MoveKey[2] != 0 && MoveKey[1] != 0) || (MoveKey[2] != 0 && MoveKey[3] != 0))
	{
		CurState->Irene->SetUseCameraLag(CurState->Irene->CameraLagCurve[2]);
	}
}

void URunLoopState::Exit(IBaseGameEntity* CurState)
{
	CurState->ThrowState(URunEndState::GetInstance());
	CurState->bIsEnd = true;
}
#pragma endregion URunLoopState
#pragma region URunEndState
URunEndState* URunEndState::GetInstance()
{
	static URunEndState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<URunEndState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void URunEndState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Run_End);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void URunEndState::Execute(IBaseGameEntity* CurState)
{
	
}

void URunEndState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->Weapon->SetVisibility(true);
	CurState->Irene->SetCameraLagTime(0);
	CurState->bIsEnd = true;
}
#pragma endregion URunEndState
#pragma endregion RunState

#pragma region SprintState
#pragma region USprintLoopState
USprintLoopState* USprintLoopState::GetInstance()
{
	static USprintLoopState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<USprintLoopState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void USprintLoopState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Sprint_Loop);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->GetCharacterMovement()->MaxWalkSpeed = CurState->Irene->IreneData.SprintMaxSpeed;
	CurState->Irene->Weapon->SetVisibility(false);
	CurState->Irene->IreneAnim->SetSprintStateAnim(true);
}

void USprintLoopState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveForward();
	CurState->Irene->IreneInput->MoveRight();
	const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
	if (MoveKey[0] == 0 && MoveKey[1] == 0 && MoveKey[2] == 0 && MoveKey[3] == 0)
	{
		// if (Irene->IreneState->GetStateToString().Compare(FString("Sprint")) == 0)
		// {
		// 	Irene->IreneAnim->SetSprintStopAnim(true);
		// 	FTimerHandle TimerHandle;
		// 	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		// 		{
		// 			Irene->IreneAnim->SetSprintStopAnim(false);
		// 		}, 0.3f, false);
		// }
		CurState->ThrowState(USprintEndState::GetInstance());
		CurState->Irene->ChangeStateAndLog(UIdleState::GetInstance());
	}
	if(MoveKey[0] != 0 || MoveKey[2] != 0)
	{
		CurState->Irene->SetUseCameraLag(CurState->Irene->CameraLagCurve[3]);
	}
	if(MoveKey[1] != 0 || MoveKey[3] != 0)
	{
		CurState->Irene->SetUseCameraLag(CurState->Irene->CameraLagCurve[4]);
	}
	if((MoveKey[0] != 0 && MoveKey[1] != 0) || (MoveKey[0] != 0 && MoveKey[3] != 0)||
		(MoveKey[2] != 0 && MoveKey[1] != 0) || (MoveKey[2] != 0 && MoveKey[3] != 0))
	{
		CurState->Irene->SetUseCameraLag(CurState->Irene->CameraLagCurve[5]);
	}
}

void USprintLoopState::Exit(IBaseGameEntity* CurState)
{
	CurState->ThrowState(USprintEndState::GetInstance());
	CurState->bIsEnd = true;
}
#pragma endregion USprintLoopState
#pragma region USprintEndState
USprintEndState* USprintEndState::GetInstance()
{
	static USprintEndState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<USprintEndState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void USprintEndState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Sprint_End);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void USprintEndState::Execute(IBaseGameEntity* CurState)
{
	
}

void USprintEndState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->Weapon->SetVisibility(true);
	CurState->Irene->IreneAnim->SetSprintStateAnim(false);
	CurState->Irene->IreneAnim->SetSprintStopAnim(false);
	CurState->Irene->SetCameraLagTime(0);
	CurState->bIsEnd = true;
}
#pragma endregion USprintEndState
#pragma endregion SprintState

#pragma region DodgeState
#pragma region UDodgeFireStartState
UDodgeFireStartState* UDodgeFireStartState::GetInstance()
{
	static UDodgeFireStartState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UDodgeFireStartState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UDodgeFireStartState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Dodge_F_Start);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void UDodgeFireStartState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	// ???? ???? ???????? ???? ?????? ???? ????
	if (CurState->Irene->GetMovementComponent()->IsFalling())
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveAutoWaitHandle);
		CurState->Irene->IreneState->SetState(UJumpLoopState::GetInstance());
		CurState->Irene->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	}
}

void UDodgeFireStartState::Exit(IBaseGameEntity* CurState)
{
	CurState->ThrowState(UDodgeFireEndState::GetInstance());
	CurState->bIsEnd = true;
}
#pragma endregion UDodgeFireStartState
#pragma region UDodgeFireEndState
UDodgeFireEndState* UDodgeFireEndState::GetInstance()
{
	static UDodgeFireEndState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UDodgeFireEndState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UDodgeFireEndState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Dodge_F_End);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void UDodgeFireEndState::Execute(IBaseGameEntity* CurState)
{
	
}

void UDodgeFireEndState::Exit(IBaseGameEntity* CurState)
{
	CurState->bIsEnd = true;
}
#pragma endregion UDodgeFireEndState
#pragma region UDodgeWaterStartState
UDodgeWaterStartState* UDodgeWaterStartState::GetInstance()
{
	static UDodgeWaterStartState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UDodgeWaterStartState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UDodgeWaterStartState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Dodge_W_Start);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->GetCharacterMovement()->MaxWalkSpeed = CurState->Irene->IreneData.SprintMaxSpeed;
	CurState->Irene->GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerDodge"));
	CurState->Irene->GetMesh()->SetVisibility(false);
	CurState->Irene->Weapon->SetVisibility(false);
}

void UDodgeWaterStartState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveForward();
	CurState->Irene->IreneInput->MoveRight();
	// ???? ???? ???????? ???? ?????? ???? ????
	if (CurState->Irene->GetMovementComponent()->IsFalling())
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveAutoWaitHandle);
		CurState->Irene->IreneState->SetState(UJumpLoopState::GetInstance());
		CurState->Irene->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	}
}

void UDodgeWaterStartState::Exit(IBaseGameEntity* CurState)
{
	CurState->ThrowState(UDodgeWaterEndState::GetInstance());
	CurState->bIsEnd = true;
}
#pragma endregion UDodgeWaterStartState
#pragma region UDodgeWaterEndState
UDodgeWaterEndState* UDodgeWaterEndState::GetInstance()
{
	static UDodgeWaterEndState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UDodgeWaterEndState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UDodgeWaterEndState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Dodge_W_End);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void UDodgeWaterEndState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->ActionEndChangeMoveState();
}

void UDodgeWaterEndState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	CurState->Irene->GetMesh()->SetVisibility(true);
	CurState->Irene->Weapon->SetVisibility(true);
	CurState->bIsEnd = true;
}
#pragma endregion UDodgeWaterEndState
#pragma region UDodgeThunderStartState
UDodgeThunderStartState* UDodgeThunderStartState::GetInstance()
{
	static UDodgeThunderStartState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UDodgeThunderStartState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UDodgeThunderStartState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Dodge_T_Start);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;	
	CurState->Irene->GetMesh()->SetVisibility(false);
	CurState->Irene->Weapon->SetVisibility(false);
	CurState->Irene->GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerDodge"));
	CurState->Irene->SetUseCameraLag(CurState->Irene->CameraLagCurve[6]);
}

void UDodgeThunderStartState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	// ???? ???? ???????? ???? ?????? ???? ????
	if (CurState->Irene->GetMovementComponent()->IsFalling())
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveAutoWaitHandle);
		CurState->Irene->IreneState->SetState(UJumpLoopState::GetInstance());
		CurState->Irene->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	}
}

void UDodgeThunderStartState::Exit(IBaseGameEntity* CurState)
{
	CurState->ThrowState(UDodgeThunderEndState::GetInstance());
	CurState->bIsEnd = true;
}
#pragma endregion UDodgeThunderStartState
#pragma region UDodgeThunderEndState
UDodgeThunderEndState* UDodgeThunderEndState::GetInstance()
{
	static UDodgeThunderEndState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UDodgeThunderEndState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UDodgeThunderEndState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Dodge_T_End);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void UDodgeThunderEndState::Execute(IBaseGameEntity* CurState)
{

}

void UDodgeThunderEndState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneAttack->SetCurrentPosVec(FVector::ZeroVector);
	CurState->Irene->IreneAttack->SetNowPosVec(FVector::ZeroVector);
	CurState->Irene->GetMesh()->SetVisibility(true);
	CurState->Irene->Weapon->SetVisibility(true);
	CurState->Irene->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	CurState->Irene->SetCameraLagTime(0);
	CurState->bIsEnd = true;
}
#pragma endregion UDodgeThunderEndState
#pragma endregion DodgeState

#pragma region JumpState
#pragma region UJumpStartState
UJumpStartState* UJumpStartState::GetInstance()
{
	static UJumpStartState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UJumpStartState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UJumpStartState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Jump_Start);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->GetMovementComponent()->Velocity = CurState->Irene->GetMovementComponent()->Velocity / CurState->Irene->IreneData.JumpDrag;
	CurState->Irene->IreneInput->SetStartJump(true);
}

void UJumpStartState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->ChangeStateAndLog(UJumpLoopState::GetInstance());
}

void UJumpStartState::Exit(IBaseGameEntity* CurState)
{
	CurState->bIsEnd = true;
}
#pragma endregion UJumpStartState
#pragma region UJumpLoopState
UJumpLoopState* UJumpLoopState::GetInstance()
{
	static UJumpLoopState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UJumpLoopState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UJumpLoopState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Jump_Loop);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurrentTime = 0;
}

void UJumpLoopState::Execute(IBaseGameEntity* CurState)
{
	const float JumpTime = CurState->Irene->IreneInput->GetJumpingTime();
	CurState->Irene->IreneInput->SetJumpingTime(JumpTime + CurState->PlayTime - CurrentTime);
	CurState->Irene->GetCharacterMovement()->GravityScale = CurState->Irene->JumpGravityCurve->GetFloatValue(JumpTime);
	CurrentTime = CurState->PlayTime;
	// ???? ???? ?? ???????? ???? ?????? ?????? ???? ????, ????, ?????? ???? ????
	if (!CurState->Irene->GetCharacterMovement()->IsFalling())
	{
		CurState->ThrowState(UJumpEndState::GetInstance());
		CurState->Irene->ActionEndChangeMoveState();
		CurState->Irene->GetCharacterMovement()->GravityScale = 2;
	}
}

void UJumpLoopState::Exit(IBaseGameEntity* CurState)
{
	CurState->bIsEnd = true;
}
#pragma endregion UJumpLoopState
#pragma region UJumpEndState
UJumpEndState* UJumpEndState::GetInstance()
{
	static UJumpEndState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UJumpEndState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UJumpEndState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Jump_End);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void UJumpEndState::Execute(IBaseGameEntity* CurState)
{
	// if (CurState->Irene->IreneInput->GetFallingRoll() && !CurState->Irene->GetMovementComponent()->IsFalling())
	// {
	// 	// ???????? ?????? ?? ???? ???? ???? ???????? ?????? ????
	// 	CurState->Irene->IreneInput->DodgeKeyword();
	// }
}

void UJumpEndState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->SetJumpingTime(0);
	CurState->Irene->IreneInput->SetStartJump(false);
	CurState->Irene->IreneInput->SetFallingRoll(false);
	CurState->bIsEnd = true;
}
#pragma endregion UJumpEndState
#pragma endregion JumpState

#pragma region AttackState
#pragma region UBasicAttack1FireState
UBasicAttack1FireState* UBasicAttack1FireState::GetInstance()
{
	static UBasicAttack1FireState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UBasicAttack1FireState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UBasicAttack1FireState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::B_Attack_1_F);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->IreneAttack->SetCameraShakeTime(0);
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[0]);
	StartShakeTime = 0.0f;
}

void UBasicAttack1FireState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	if(CurState->Irene->IreneAnim->Montage_GetCurrentSection(CurState->Irene->IreneAnim->GetCurrentActiveMontage()) == FName("Attack2")
		&& CurState->Irene->IreneState->GetStateToString().Compare(FString("B_Attack_2_F")) != 0)
	{
		CurState->Irene->IreneAttack->SetAttackState();
	}
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;
	
	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}	
}

void UBasicAttack1FireState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->bIsEnd = true;
}
#pragma endregion UBasicAttack1FireState
#pragma region UBasicAttack2FireState
UBasicAttack2FireState* UBasicAttack2FireState::GetInstance()
{
	static UBasicAttack2FireState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UBasicAttack2FireState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UBasicAttack2FireState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::B_Attack_2_F);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->IreneAttack->SetCameraShakeTime(0);
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[1]);
	StartShakeTime = 0.0f;
}

void UBasicAttack2FireState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	if(CurState->Irene->IreneAnim->Montage_GetCurrentSection(CurState->Irene->IreneAnim->GetCurrentActiveMontage()) == FName("Attack3")
		&& CurState->Irene->IreneState->GetStateToString().Compare(FString("B_Attack_3_F")) != 0)
	{
		CurState->Irene->IreneAttack->SetAttackState();
	}
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;

	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void UBasicAttack2FireState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->bIsEnd = true;
}
#pragma endregion UBasicAttack2FireState
#pragma region UBasicAttack3FireState
UBasicAttack3FireState* UBasicAttack3FireState::GetInstance()
{
	static UBasicAttack3FireState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UBasicAttack3FireState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UBasicAttack3FireState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::B_Attack_3_F);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->IreneAttack->SetCameraShakeTime(0);
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[2]);
	StartShakeTime = 0.0f;
}

void UBasicAttack3FireState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;

	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void UBasicAttack3FireState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->bIsEnd = true;
}
#pragma endregion UBasicAttack3FireState
#pragma region UBasicAttack1WaterState
UBasicAttack1WaterState* UBasicAttack1WaterState::GetInstance()
{
	static UBasicAttack1WaterState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UBasicAttack1WaterState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UBasicAttack1WaterState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::B_Attack_1_W);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->IreneAttack->SetCameraShakeTime(0);
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[3]);
	StartShakeTime = 0.0f;
}

void UBasicAttack1WaterState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	
	if(CurState->Irene->IreneAnim->Montage_GetCurrentSection(CurState->Irene->IreneAnim->GetCurrentActiveMontage()) == FName("Attack2")
		&& CurState->Irene->IreneState->GetStateToString().Compare(FString("B_Attack_2_W")) != 0)
	{
		CurState->Irene->IreneAttack->SetAttackState();
	}
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;

	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void UBasicAttack1WaterState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->bIsEnd = true;
}
#pragma endregion UBasicAttack1WaterState
#pragma region UBasicAttack2WaterState
UBasicAttack2WaterState* UBasicAttack2WaterState::GetInstance()
{
	static UBasicAttack2WaterState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UBasicAttack2WaterState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UBasicAttack2WaterState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::B_Attack_2_W);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->IreneAttack->SetCameraShakeTime(0);
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[4]);
	StartShakeTime = 0.0f;
}

void UBasicAttack2WaterState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	
	if(CurState->Irene->IreneAnim->Montage_GetCurrentSection(CurState->Irene->IreneAnim->GetCurrentActiveMontage()) == FName("Attack3")
		&& CurState->Irene->IreneState->GetStateToString().Compare(FString("B_Attack_3_W")) != 0)
	{
		CurState->Irene->IreneAttack->SetAttackState();
	}
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;
	
	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void UBasicAttack2WaterState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->bIsEnd = true;
}
#pragma endregion UBasicAttack2WaterState
#pragma region UBasicAttack3WaterState
UBasicAttack3WaterState* UBasicAttack3WaterState::GetInstance()
{
	static UBasicAttack3WaterState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UBasicAttack3WaterState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UBasicAttack3WaterState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::B_Attack_3_W);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->IreneAttack->SetCameraShakeTime(0);
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[5]);
	StartShakeTime = 0.0f;
}

void UBasicAttack3WaterState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;

	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void UBasicAttack3WaterState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->bIsEnd = true;
}
#pragma endregion UBasicAttack3WaterState
#pragma region UBasicAttack1ThunderState
UBasicAttack1ThunderState* UBasicAttack1ThunderState::GetInstance()
{
	static UBasicAttack1ThunderState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UBasicAttack1ThunderState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UBasicAttack1ThunderState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::B_Attack_1_T);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->IreneAttack->SetCameraShakeTime(0);
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[6]);
	StartShakeTime = 0.0f;
}

void UBasicAttack1ThunderState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	
	if(CurState->Irene->IreneAnim->Montage_GetCurrentSection(CurState->Irene->IreneAnim->GetCurrentActiveMontage()) == FName("Attack2")
		&& CurState->Irene->IreneState->GetStateToString().Compare(FString("B_Attack_2_T")) != 0)
	{
		CurState->Irene->IreneAttack->SetAttackState();
	}
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;

	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void UBasicAttack1ThunderState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->bIsEnd = true;
}
#pragma endregion UBasicAttack1ThunderState
#pragma region UBasicAttack2ThunderState
UBasicAttack2ThunderState* UBasicAttack2ThunderState::GetInstance()
{
	static UBasicAttack2ThunderState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UBasicAttack2ThunderState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UBasicAttack2ThunderState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::B_Attack_2_T);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->IreneAttack->SetCameraShakeTime(0);
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[7]);
	StartShakeTime = 0.0f;
}

void UBasicAttack2ThunderState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	
	if(CurState->Irene->IreneAnim->Montage_GetCurrentSection(CurState->Irene->IreneAnim->GetCurrentActiveMontage()) == FName("Attack3")
	&& CurState->Irene->IreneState->GetStateToString().Compare(FString("B_Attack_3_T")) != 0)
	{
		CurState->Irene->IreneAttack->SetAttackState();
	}
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;

	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void UBasicAttack2ThunderState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->bIsEnd = true;
}
#pragma endregion UBasicAttack2ThunderState
#pragma region UBasicAttack3ThunderState
UBasicAttack3ThunderState* UBasicAttack3ThunderState::GetInstance()
{
	static UBasicAttack3ThunderState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UBasicAttack3ThunderState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UBasicAttack3ThunderState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::B_Attack_3_T);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->IreneAttack->SetCameraShakeTime(0);
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[8]);
	StartShakeTime = 0.0f;
}

void UBasicAttack3ThunderState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;

	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void UBasicAttack3ThunderState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->bIsEnd = true;
}
#pragma endregion UBasicAttack3ThunderState
#pragma endregion AttackState

#pragma region SkillState
#pragma region USkillFireStartState
USkillFireStartState* USkillFireStartState::GetInstance()
{
	static USkillFireStartState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<USkillFireStartState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void USkillFireStartState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Skill_F_Start);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[9]);
	StartShakeTime = 0.0f;
	CurState->Irene->IreneData.IsAttacking = true;
	CurState->Irene->IreneData.CanNextCombo = true;
}

void USkillFireStartState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;

	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void USkillFireStartState::Exit(IBaseGameEntity* CurState)
{
	CurState->ThrowState(USkillFireEndState::GetInstance());
	CurState->bIsEnd = true;
}
#pragma endregion USkillFireStartState
#pragma region USkillFireEndState
USkillFireEndState* USkillFireEndState::GetInstance()
{
	static USkillFireEndState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<USkillFireEndState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void USkillFireEndState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Skill_F_End);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void USkillFireEndState::Execute(IBaseGameEntity* CurState)
{
	
}

void USkillFireEndState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->Irene->IreneData.IsAttacking = false;
	CurState->Irene->IreneData.CanNextCombo = false;
	CurState->bIsEnd = true;
}
#pragma endregion USkillFireEndState
#pragma region USkillWaterStartState
USkillWaterStartState* USkillWaterStartState::GetInstance()
{
	static USkillWaterStartState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<USkillWaterStartState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void USkillWaterStartState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Skill_W_Start);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[10]);
	StartShakeTime = 0.0f;
	CurState->Irene->IreneData.IsAttacking = true;
	CurState->Irene->IreneData.CanNextCombo = true;
}

void USkillWaterStartState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	
	if(CurState->PlayTime >= 1.77f)
		CurState->Irene->ActionEndChangeMoveState();
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;

	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void USkillWaterStartState::Exit(IBaseGameEntity* CurState)
{
	CurState->ThrowState(USkillWaterEndState::GetInstance());
	CurState->bIsEnd = true;
}
#pragma endregion USkillWaterStartState
#pragma region USkillWaterEndState
USkillWaterEndState* USkillWaterEndState::GetInstance()
{
	static USkillWaterEndState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<USkillWaterEndState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void USkillWaterEndState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Skill_W_End);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void USkillWaterEndState::Execute(IBaseGameEntity* CurState)
{
	
}

void USkillWaterEndState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->CameraShakeOn = false;
	CurState->Irene->IreneData.IsAttacking = false;
	CurState->Irene->IreneData.CanNextCombo = false;
	CurState->bIsEnd = true;
}
#pragma endregion USkillWaterEndState
#pragma region USkillThunderStartState
USkillThunderStartState* USkillThunderStartState::GetInstance()
{
	static USkillThunderStartState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<USkillThunderStartState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void USkillThunderStartState::Enter(IBaseGameEntity* CurState)
{
	STARRYLOG_S(Error);
	CurState->SetStateEnum(EStateEnum::Skill_T_Start);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	const FVector CurrentPosVec = CurState->Irene->GetActorLocation();
	const FVector NowPosVec = CurState->Irene->GetActorLocation()+CurState->Irene->GetActorForwardVector()*400;
	CurState->Irene->IreneInput->SetStartMoveAutoTarget(CurrentPosVec, NowPosVec);
	CurState->Irene->IreneAttack->SetCurrentPosVec(CurrentPosVec);
	CurState->Irene->IreneAttack->SetNowPosVec(NowPosVec);
	CurState->Irene->GetMesh()->SetVisibility(false);
	CurState->Irene->Weapon->SetVisibility(false);
	CurState->Irene->GetCapsuleComponent()->SetCollisionProfileName(TEXT("PlayerDodge"));
	CurState->Irene->SetUseShakeCurve(CurState->Irene->CameraShakeCurve[11]);
	StartShakeTime = 0.0f;
	CurState->Irene->IreneData.IsAttacking = true;
	CurState->Irene->IreneData.CanNextCombo = true;
}

void USkillThunderStartState::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveAuto();
	
	if(CurState->PlayTime >= 0.7f)
		CurState->Irene->ActionEndChangeMoveState();
	
	if(CurState->Irene->CameraShakeOn == true && StartShakeTime == 0)
		StartShakeTime = CurState->PlayTime;
	if(StartShakeTime != 0 && CurState->PlayTime >= StartShakeTime + 0.2f)
		CurState->Irene->CameraShakeOn = false;

	if (CurState->Irene->IreneData.IsAttacking)
	{
		const TArray<uint8> MoveKey = CurState->Irene->IreneInput->MoveKey;
		if (!CurState->Irene->IreneData.CanNextCombo && (MoveKey[0] != 0 || MoveKey[1] != 0 || MoveKey[2] != 0 || MoveKey[3] != 0))
		{
			CurState->Irene->IreneAnim->StopAllMontages(0);
			CurState->Irene->ChangeStateAndLog(URunLoopState::GetInstance());
		}
	}
}

void USkillThunderStartState::Exit(IBaseGameEntity* CurState)
{
	CurState->ThrowState(USkillThunderEndState::GetInstance());
	CurState->bIsEnd = true;
}
#pragma endregion USkillThunderStartState
#pragma region USkillThunderEndState
USkillThunderEndState* USkillThunderEndState::GetInstance()
{
	static USkillThunderEndState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<USkillThunderEndState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void USkillThunderEndState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Skill_T_End);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void USkillThunderEndState::Execute(IBaseGameEntity* CurState)
{
	
}

void USkillThunderEndState::Exit(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneAttack->SetCurrentPosVec(FVector::ZeroVector);
	CurState->Irene->IreneAttack->SetNowPosVec(FVector::ZeroVector);
	CurState->Irene->GetMesh()->SetVisibility(true);
	CurState->Irene->Weapon->SetVisibility(true);
	CurState->Irene->CameraShakeOn = false;
	CurState->Irene->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
	CurState->Irene->IreneData.IsAttacking = false;
	CurState->Irene->IreneData.CanNextCombo = false;
	CurState->bIsEnd = true;
}
#pragma endregion USkillThunderEndState
#pragma endregion SkillState

#pragma region Charge
#pragma region UCharge1State
UCharge1State* UCharge1State::GetInstance()
{
	static UCharge1State* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UCharge1State>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UCharge1State::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Charge_1);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurrentTime = 0;
}

void UCharge1State::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->SetDeltaTimeChargingTime(CurState->PlayTime - CurrentTime);
	CurrentTime = CurState->PlayTime;
}

void UCharge1State::Exit(IBaseGameEntity* CurState)
{
	CurState->bIsEnd = true;
}
#pragma endregion UCharge1State
#pragma region UCharge2State
UCharge2State* UCharge2State::GetInstance()
{
	static UCharge2State* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UCharge2State>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UCharge2State::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Charge_2);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurrentTime = 0;
}

void UCharge2State::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->SetDeltaTimeChargingTime(CurState->PlayTime - CurrentTime);
	CurrentTime = CurState->PlayTime;
}

void UCharge2State::Exit(IBaseGameEntity* CurState)
{
	CurState->bIsEnd = true;
}
#pragma endregion UCharge2State
#pragma endregion Charge

#pragma region UFormChangeState
UFormChangeState* UFormChangeState::GetInstance()
{
	static UFormChangeState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UFormChangeState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UFormChangeState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Form_Change);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void UFormChangeState::Execute(IBaseGameEntity* CurState)
{
	
}

void UFormChangeState::Exit(IBaseGameEntity* CurState)
{
	CurState->bIsEnd = true;
}
#pragma endregion UFormChangeState

#pragma region Hit
#pragma region UHit1State
UHit1State* UHit1State::GetInstance()
{
	static UHit1State* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UHit1State>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UHit1State::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Hit_1);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void UHit1State::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveForward();
	CurState->Irene->IreneInput->MoveRight();
	if(CurState->PlayTime >= 0.1f)
		CurState->Irene->ActionEndChangeMoveState();
}

void UHit1State::Exit(IBaseGameEntity* CurState)
{
	CurState->bIsEnd = true;
}
#pragma endregion UHit1State
#pragma region UHit2State
UHit2State* UHit2State::GetInstance()
{
	static UHit2State* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UHit2State>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UHit2State::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Hit_2);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
}

void UHit2State::Execute(IBaseGameEntity* CurState)
{
	CurState->Irene->IreneInput->MoveForward();
	CurState->Irene->IreneInput->MoveRight();
}

void UHit2State::Exit(IBaseGameEntity* CurState)
{
	CurState->bIsEnd = true;
}
#pragma endregion UHit2State
#pragma endregion Hit

#pragma region UDeathState
UDeathState* UDeathState::GetInstance()
{
	static UDeathState* Instance;
	if (Instance == nullptr) {
		Instance = NewObject<UDeathState>();
		Instance->AddToRoot();
	}
	return Instance;
}
void UDeathState::Enter(IBaseGameEntity* CurState)
{
	CurState->SetStateEnum(EStateEnum::Death);
	CurState->PlayTime = 0.0f;
	CurState->bIsEnd = false;
	CurState->Irene->IreneAnim->StopAllMontages(0);
	CurState->Irene->IreneAnim->SetDeadAnim(true);
}

void UDeathState::Execute(IBaseGameEntity* CurState)
{
	
}

void UDeathState::Exit(IBaseGameEntity* CurState)
{
	CurState->bIsEnd = true;
}
#pragma endregion UDeathState

#pragma region IsState
bool UIreneFSM::IsIdleState()const
{
	if(StateEnumValue == EStateEnum::Idle || StateEnumValue == EStateEnum::BattleIdle)
		return true;
	return false;
}
bool UIreneFSM::IsRunState()const
{
	if(StateEnumValue == EStateEnum::Run_Loop || StateEnumValue == EStateEnum::Run_End)
		return true;
	return false;
}
bool UIreneFSM::IsSprintState()const
{
	if(StateEnumValue == EStateEnum::Sprint_Loop || StateEnumValue == EStateEnum::Sprint_End)
		return true;
	return false;
}
bool UIreneFSM::IsDodgeState()const
{
	if(StateEnumValue == EStateEnum::Dodge_F_Start || StateEnumValue == EStateEnum::Dodge_F_End ||
	StateEnumValue == EStateEnum::Dodge_W_Start || StateEnumValue == EStateEnum::Dodge_W_End ||
	StateEnumValue == EStateEnum::Dodge_T_Start || StateEnumValue == EStateEnum::Dodge_T_End)
		return true;
	return false;
}
bool UIreneFSM::IsJumpState()const
{
	if(StateEnumValue == EStateEnum::Jump_Start || StateEnumValue == EStateEnum::Jump_Loop || StateEnumValue == EStateEnum::Jump_End)
		return true;
	return false;
}
bool UIreneFSM::IsAttackState()const
{
	if(StateEnumValue == EStateEnum::B_Attack_1_F || StateEnumValue == EStateEnum::B_Attack_1_W || StateEnumValue == EStateEnum::B_Attack_1_T ||
	StateEnumValue == EStateEnum::B_Attack_2_F || StateEnumValue == EStateEnum::B_Attack_2_W || StateEnumValue == EStateEnum::B_Attack_2_T ||
	StateEnumValue == EStateEnum::B_Attack_3_F || StateEnumValue == EStateEnum::B_Attack_3_W || StateEnumValue == EStateEnum::B_Attack_3_T)
		return true;
	return false;
}
bool UIreneFSM::IsSkillState()const
{
	if(StateEnumValue == EStateEnum::Skill_F_Start || StateEnumValue == EStateEnum::Skill_W_Start || StateEnumValue == EStateEnum::Skill_T_Start ||
	StateEnumValue == EStateEnum::Skill_F_End || StateEnumValue == EStateEnum::Skill_W_End || StateEnumValue == EStateEnum::Skill_T_End)
		return true;
	return false;
}
bool UIreneFSM::IsChargeState()const
{
	if(StateEnumValue == EStateEnum::Charge_1 || StateEnumValue == EStateEnum::Charge_2)
		return true;
	return false;
}
bool UIreneFSM::IsFormChangeState()const
{
	if(StateEnumValue == EStateEnum::Form_Change)
		return true;
	return false;
}
bool UIreneFSM::IsHitState()const
{
	if(StateEnumValue == EStateEnum::Hit_1 || StateEnumValue == EStateEnum::Hit_2)
		return true;
	return false;
}
bool UIreneFSM::IsDeathState()const
{
	if(StateEnumValue == EStateEnum::Death)
		return true;
	return false;
}
#pragma endregion IsState

#pragma region FindState
bool UIreneFSM::IsFirstAttack()const
{
	if(StateEnumValue == EStateEnum::B_Attack_1_F || StateEnumValue == EStateEnum::B_Attack_1_W || StateEnumValue == EStateEnum::B_Attack_1_T)
		return true;
	return false;
}
#pragma endregion  FindState