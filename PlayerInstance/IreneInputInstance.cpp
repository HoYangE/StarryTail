﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "IreneInputInstance.h"

#include "../IreneCharacter.h"
#include "../PlayerFSM/IreneFSM.h"
#include "IreneAnimInstance.h"
#include "IreneAttackInstance.h"
#include "IreneUIManager.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "../../STGameInstance.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetMathLibrary.h"
#include "../PlayerSpirit/IreneSpirit.h"
#include "../PlayerSpirit/IreneSpiritAnimInstance.h"
#include "../../UI/IngameWidget_D.h"
#include "StarryTail/PlayerSource/HeliosInstance/HeliosAnimInstance.h"

UIreneInputInstance::UIreneInputInstance()
{
	const ConstructorHelpers::FObjectFinder<UParticleSystem> BreakAttackStartEffectObject(TEXT("/Game/Effect/VFX_Dodge/Ps_Light_Spawn.Ps_Light_Spawn"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> BreakAttackEndEffectObject(TEXT("/Game/Effect/VFX_Dodge/Ps_Light_End.Ps_Light_End"));
	if(BreakAttackStartEffectObject.Succeeded() && BreakAttackEndEffectObject.Succeeded())
	{
		BreakAttackStartEffect = BreakAttackStartEffectObject.Object;
		BreakAttackEndEffect = BreakAttackEndEffectObject.Object;
	}
	
	const ConstructorHelpers::FObjectFinder<ULevelSequence> UltimateAttackSequenceObject(TEXT("/Game/Movies/NewLevelSequence.NewLevelSequence"));
	if(UltimateAttackSequenceObject.Succeeded())
	{
		UltimateAttackSequence = UltimateAttackSequenceObject.Object;
	}
	
	bIsSpiritChangeEnable = true;
}

void UIreneInputInstance::Init(AIreneCharacter* Value)
{
	SetIreneCharacter(Value);
	InitMemberVariable();
}
void UIreneInputInstance::SetIreneCharacter(AIreneCharacter* Value)
{
	Irene = Value;
}
void UIreneInputInstance::InitMemberVariable()
{
	// WASD 키 입력 초기화
	MoveKey.Add(0);
	MoveKey.Add(0);
	MoveKey.Add(0);
	MoveKey.Add(0);

	bLeftButtonPressed = false;
	bRightButtonPressed = false;

	bCameraStop = false;
	
	bReAttack = false;
	
	IsFallingRoll = false;
	bIsStun = false;
	
	MaxDodgeCoolTime = 0.5f;
	DodgeCoolTime = 0.0f;
	bIsDodgeOn = true;
	MaxDodgeCount = 2;
	DodgeCount = MaxDodgeCount;	
	bIsDodgeToDodge = false;
	bPerfectDodgeToAttack = false;
	
	SpiritChainAttackCount = 0;
	
	CurSoulValue = 0.0f;

	MaxSwordSkillCoolTime = 0.0f;
	SwordSkillCoolTime = 0.0f;
	CanSwordSkill2Time = 2.0f;

	SpiritSpawnCoolTime = 0.1f;
	SpiritAnimJumpCoolTime = 1.5f;
	
	bIsThunderAttributeOn = true;

	bIsSkillOn = false;
	bAttackUseSkill = false;
	CanUseSecondSwordSkill = false;
	CanUseSpiritAnimJump = false;
	
	bSkillCameraMove = false;
	MaxSkillCameraPlayTime = 2.0f;
	
	CoolTimeRate = 0.008f;
	SlowScale = 0.4f;
	bIsDialogOn = false;
}
void UIreneInputInstance::Begin()
{
	const TUniquePtr<FElementDataTable> ThunderFormTimeDataTable = MakeUnique<FElementDataTable>(*Irene->IreneAttack->GetNameAtElementDataTable(FName("Thunder_Ele")));
	if(ThunderFormTimeDataTable != nullptr)
		MaxSoulValue = ThunderFormTimeDataTable->Ele_C_Time;

	LevelSequenceActor = GetWorld()->SpawnActor<ALevelSequenceActor>(ALevelSequenceActor::StaticClass());
	PlaybackSettings.bAutoPlay = false;
	PlaybackSettings.bRandomStartTime = false;
	PlaybackSettings.bRestoreState = false;
	PlaybackSettings.bDisableMovementInput = false;
	PlaybackSettings.bDisableLookAtInput = false;
	PlaybackSettings.bHidePlayer = false;
	PlaybackSettings.bHideHud = false;
	PlaybackSettings.bDisableCameraCuts = false;
	PlaybackSettings.bPauseAtEnd = false;
	PlaybackSettings.PlayRate = 10.0f;
	PlaybackSettings.StartTime = 0.0f;	
	LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), 
		UltimateAttackSequence,
		PlaybackSettings, 
		LevelSequenceActor
	);
	LevelSequencePlayer->OnFinished.AddDynamic(this, &UIreneInputInstance::SkillCameraMoveEnd);
}

#pragma region Move
void UIreneInputInstance::MoveForward()
{
	const FRotator Rotation = Irene->Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// 0: 전진, 2: 후진
	if (MoveKey[0] != 0 && MoveKey[0] < 3)
		Irene->AddMovementInput(Direction);
	if (MoveKey[2] != 0 && MoveKey[2] < 3)
		Irene->AddMovementInput(Direction * -1);
}
void UIreneInputInstance::MoveRight()
{
	const FRotator Rotation = Irene->Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	// 1: 좌측, 3: 우측
	if (MoveKey[1] != 0 && MoveKey[1] < 3)
		Irene->AddMovementInput(Direction * -1);
	if (MoveKey[3] != 0 && MoveKey[3] < 3)
		Irene->AddMovementInput(Direction);
}

void UIreneInputInstance::MoveAuto(const float EndTimer)const
{
	// 자동으로 이동시키는 함수
	if (Irene->IreneAttack->GetFollowTarget())
	{		
		const float TargetAlpha = Irene->IreneAttack->GetFollowTargetAlpha()/EndTimer;
		Irene->IreneAttack->SetFollowTargetAlpha((TargetAlpha + GetWorld()->GetDeltaSeconds() * 2 * Irene->IreneData.TargetFollowSpeed)/EndTimer);
		if (TargetAlpha >= EndTimer)
		{
			Irene->IreneAttack->SetFollowTargetAlpha(1);
			SetStopMoveAutoTarget();
			return;
		}
		const FVector Target = FMath::Lerp(Irene->IreneAttack->GetPlayerPosVec(), Irene->IreneAttack->GetTargetPosVec(), Irene->IreneAttack->GetFollowTargetAlpha());
		Irene->GetCapsuleComponent()->SetRelativeLocation(Target, true);
	}
}
#pragma endregion Move

#pragma region MoveInput
void UIreneInputInstance::MovePressedKey(const int Value)
{
	if (!bIsDialogOn)
	{
		// 런 상태로 전이가 가능한 상태에서 키를 입력하면 1, 스프린트 속도에서 키를 입력하면 2, 런 상태가 불가능한 상태에서 키를 입력하면 3
		// 3은 나중에 AIreneCharacter::ActionEndChangeMoveState에서 1로 적용
		if (CanRunState() && !bIsStun)
		{
			MoveKey[Value] = 1;
			if (Irene->GetCharacterMovement()->MaxWalkSpeed == Irene->IreneData.SprintMaxSpeed)
				MoveKey[Value] = 2;
			if (Irene->IreneState->IsIdleState())
			{
				// 반대 방향키 아니면
				if (!((MoveKey[0] != 0 && MoveKey[2] != 0) || (MoveKey[1] != 0 && MoveKey[3] != 0)))
				{
					Irene->GetCharacterMovement()->MaxWalkSpeed = Irene->IreneData.RunMaxSpeed;
					Irene->ChangeStateAndLog(URunLoopState::GetInstance());
				}
			}
		}
		else
		{
			MoveKey[Value] = 3;			
		}
	}
	else
	{
		MoveKey[Value] = 3;
		Irene->ChangeStateAndLog(UIdleState::GetInstance());
	}
}

void UIreneInputInstance::MoveW(float Rate)
{
	if(Rate >= 1 && !Irene->bInputStop)
		MovePressedKey(0);
	else
		MoveKey[0] = 0;
}
void UIreneInputInstance::MoveA(float Rate)
{
	if(Rate >= 1 && !Irene->bInputStop)
		MovePressedKey(1);
	else
		MoveKey[1] = 0;
}
void UIreneInputInstance::MoveS(float Rate)
{
	if(Rate >= 1 && !Irene->bInputStop)
		MovePressedKey(2);
	else
		MoveKey[2] = 0;
}
void UIreneInputInstance::MoveD(float Rate)
{
	if(Rate >= 1 && !Irene->bInputStop)
		MovePressedKey(3);
	else
		MoveKey[3] = 0;
}
FVector UIreneInputInstance::GetMoveKeyToDirVector()
{
	// 현재 키에 따라 방향 리턴 함수
	const FRotator Rotation = Irene->Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector LookDir = FVector::ZeroVector;
	if (MoveKey[0] != 0 && MoveKey[0] <= 3)
		LookDir += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	if (MoveKey[2] != 0 && MoveKey[2] <= 3)
		LookDir += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X)*-1;
	if (MoveKey[1] != 0 && MoveKey[1] <= 3)
		LookDir += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y)*-1;
	if (MoveKey[3] != 0 && MoveKey[3] <= 3)
		LookDir += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	if(LookDir == FVector::ZeroVector)
		LookDir = Irene->GetActorForwardVector();
	LookDir.Normalize();

	return LookDir;
}
int UIreneInputInstance::GetMoveKeyToDirNumber()
{
	int LookDir = 0;
	auto Temp = MoveKey;
	
	if(Temp[0] != 0 && Temp[2] != 0)
	{
		Temp[0] = 0;
		Temp[2] = 0;
	}
	if(Temp[1] != 0 && Temp[3] != 0)
	{
		Temp[1] = 0;
		Temp[3] = 0;
	}
	
	if (Temp[0] != 0 && Temp[1] == 0 && Temp[3] == 0)
		LookDir = 0;
	else if(Temp[0] != 0 && Temp[1] == 0 && Temp[3] != 0)
		LookDir = 1;
	else if(Temp[0] == 0 && Temp[1] == 0 && Temp[3] != 0)
		LookDir = 2;
	else if(Temp[2] != 0 && Temp[1] == 0 && Temp[3] != 0)
		LookDir = 3;
	else if(Temp[2] != 0 && Temp[1] == 0 && Temp[3] == 0)
		LookDir = 4;
	else if(Temp[2] != 0 && Temp[1] != 0 && Temp[3] == 0)
		LookDir = 5;
	else if(Temp[2] == 0 && Temp[1] != 0 && Temp[3] == 0)
		LookDir = 6;
	else if(Temp[0] != 0 && Temp[1] != 0 && Temp[3] == 0)
		LookDir = 7;

	return LookDir;
}
#pragma endregion MoveInput

#pragma region Input
void UIreneInputInstance::Turn(float Rate)
{
	// 마우스 좌우 이동
	if (Irene->WorldController->bShowMouseCursor == false && !Irene->IreneState->IsDeathState() && !Irene->bInputStop)
		Irene->AddControllerYawInput(Rate * Irene->IreneData.EDPI / 2);
}
void UIreneInputInstance::LookUp(float Rate)
{
	// 마우스 상하 이동
	if(!Irene->bInputStop)
	{
		const float Yaw = FRotator::NormalizeAxis(Irene->WorldController->GetControlRotation().Pitch) + Rate * Irene->IreneData.EDPI * Irene->WorldController->InputPitchScale;

		// 시야각 제한
		if (Yaw < 50)
		{
			if (Irene->WorldController->bShowMouseCursor == false && !Irene->IreneState->IsDeathState())
				Irene->AddControllerPitchInput(Rate * Irene->IreneData.EDPI / 5.5f);
		}
	}
}

void UIreneInputInstance::LeftButton(float Rate)
{
	if(Rate > 0)
		bLeftButtonPressed = true;
	else
		bLeftButtonPressed = false;
	if ((CanAttackState() || (Irene->IreneState->IsSkillState() && bReAttack) || (Irene->IreneState->IsSkillState() && Irene->IreneAttack->GetCanSkillToAttack())) &&
		!AttackWaitHandle.IsValid() && !bIsDialogOn && !Irene->bInputStop && !bIsStun)
	{
		if (Rate >= 1.0)
		{
			// if(Irene->IreneState->IsSkillState() && Irene->IreneAttack->GetCanSkillToAttack())
			// {
			// 	if(Irene->IreneSpirit != nullptr)
			// 	{
			// 		Irene->SetActorLocation(Irene->IreneSpirit->GetActorLocation());
			// 		Irene->IreneSpirit->DestroySpirit();
			// 	}
			// }
			if(Irene->IreneAnim->GetDodgeDir() != 10)
			{
				if(AttackUseSkillNextCount==0)
					Irene->IreneAttack->SetAttackState();
				else
				{
					switch (AttackUseSkillNextCount)
					{
					case 1:
						Irene->ChangeStateAndLog(UBasicAttack1State::GetInstance());
						break;
					case 2:
						Irene->ChangeStateAndLog(UBasicAttack2State::GetInstance());
						break;
					case 3:
						Irene->ChangeStateAndLog(UBasicAttack3State::GetInstance());
						break;
					default:
						break;
					}
				}
				const TUniquePtr<FAttackDataTable> AttackTable = MakeUnique<FAttackDataTable>(*Irene->IreneAttack->GetNameAtAttackDataTable(Irene->IreneAttack->GetBasicAttackDataTableName()));
			
				// 공격력 계산으로 기본적으로 ATTACK_DAMAGE_1만 사용함
				if(AttackTable != nullptr)
					Irene->IreneData.Strength = AttackTable->ATTACK_DAMAGE_1;				
			}

			// 마우스 왼쪽 누르고 있을 때 연속공격 지연 시간(짧은 시간에 여러번 공격 인식 안하도록 함)
			constexpr float WaitTime = 0.05f;
			GetWorld()->GetTimerManager().SetTimer(AttackWaitHandle, FTimerDelegate::CreateLambda([&]()
				{
						GetWorld()->GetTimerManager().ClearTimer(AttackWaitHandle);
				}), WaitTime, false);

			if(AttackUseSkillNextCount>0)
			{
				Irene->IreneAnim->StopAllMontages(0);
				Irene->IreneData.CurrentCombo = AttackUseSkillNextCount;				
				AttackUseSkillNextCount = 0;
				Irene->IreneData.IsAttacking = true;
				Irene->IreneData.CanNextCombo = false;
				Irene->IreneData.IsComboInputOn = false;
				if(Irene->IreneAnim->GetCurrentActiveMontage() == nullptr)
					Irene->IreneAnim->PlayAttackMontage();
				else
					Irene->IreneAnim->JumpToAttackMontageSection(Irene->IreneData.CurrentCombo);
				GetWorld()->GetTimerManager().ClearTimer(AttackUseSkillNextCountWaitHandle);
				return;
			}
			
			if (Irene->IreneData.IsAttacking)
			{
				if(bNextAttack)
				{
					if (Irene->IreneData.CanNextCombo)
						Irene->IreneData.IsComboInputOn = true;
				}
				if(bJumpAttack)
				{
					Irene->IreneAttack->AttackStartComboState();
					Irene->IreneAnim->JumpToAttackMontageSection(Irene->IreneData.CurrentCombo);
				}
				if(bReAttack)
				{
					if(Irene->IreneState->IsSkillState())
						Irene->IreneAnim->StopAllMontages(0);
					Irene->ChangeStateAndLog(UBasicAttack1State::GetInstance());
					Irene->IreneData.IsAttacking = true;
					Irene->IreneData.CurrentCombo = 0;
					Irene->IreneAttack->AttackStartComboState();
					if(Irene->IreneAnim->GetCurrentActiveMontage() == nullptr)
						Irene->IreneAnim->PlayAttackMontage();
					else
						Irene->IreneAnim->Montage_JumpToSection(FName("Attack1"), Irene->IreneAnim->GetCurrentActiveMontage());
				}
			}
			else
			{
				if(Irene->IreneAnim->GetDodgeDir() == 10)
				{
					Irene->CustomTimeDilation = 4.0f;
					bPerfectDodgeToAttack = true;
				}
				else
				{
					Irene->IreneAttack->AttackStartComboState();
					Irene->IreneAnim->PlayAttackMontage();
					Irene->IreneAnim->NextToAttackMontageSection(Irene->IreneData.CurrentCombo);
					Irene->IreneData.IsAttacking = true;					
				}
			}
		}
	}
}
#pragma region Skill
void UIreneInputInstance::RightButton(float Rate)
{
	// UI작성 시 주석 모두 제거
	if(Rate > 0)
		bRightButtonPressed = true;
	else
		bRightButtonPressed = false;
	if ((CanSkillState()||Irene->IreneAttack->GetCanSkillSkip()) && !SwordSkillWaitHandle.IsValid() && !bIsDialogOn && !Irene->bInputStop && !bIsStun)
	{
		if (Rate >= 1.0)
		{
			// 마우스 오른쪽 누르고 있을 때 연속공격 지연 시간(짧은 시간에 여러번 공격 인식 안하도록 함)
			constexpr float WaitTime = 0.05f;
			GetWorld()->GetTimerManager().SetTimer(SwordSkillWaitHandle, FTimerDelegate::CreateLambda([&]()
			{
					GetWorld()->GetTimerManager().ClearTimer(SwordSkillWaitHandle);
				}), WaitTime, false);

			// X번째 일반 공격 중 스킬 사용 후 다시 일반 공격하면 X+1번째 공격 하도록 지정
			if(Irene->IreneAttack->GetCanSkillSkip())
			{
				bAttackUseSkill = true;
				AttackUseSkillNextCount = Irene->IreneData.CurrentCombo+1;
				if(AttackUseSkillNextCount>Irene->IreneData.MaxCombo)
					AttackUseSkillNextCount = 1;
				GetWorld()->GetTimerManager().SetTimer(AttackUseSkillNextCountWaitHandle, FTimerDelegate::CreateLambda([&]()
				{
					AttackUseSkillNextCount = 0;
					GetWorld()->GetTimerManager().ClearTimer(AttackUseSkillNextCountWaitHandle);
				}), 2, false);
			}

			if(!Irene->bIsSpiritStance)
				Irene->IreneAnim->StopAllMontages(0);

			if(Irene->bIsSpiritStance)
			{
				if(Irene->IreneSpirit == nullptr || !Irene->IreneState->IsSkillState())
				{
					if(Irene->IreneSpirit != nullptr)
					{
						//Irene->IreneSpirit->DestroySpirit();
						//Irene->IreneSpirit = nullptr;
					}
					const TUniquePtr<FAttackDataTable> AttackTable = MakeUnique<FAttackDataTable>(*Irene->IreneAttack->GetNameAtAttackDataTable(Irene->IreneAttack->GetBasicAttackDataTableName()));
					
					// 카메라 위치를 기반으로 박스를 만들어서 몬스터들을 탐지하는 방법	
					auto AllPosition = Irene->SetCameraStartTargetPosition(FVector(50,50,AttackTable->Attack_Distance_2),Irene->CameraComp->GetComponentLocation());
					auto HitMonsterList = Irene->StartPositionFindNearMonster(AllPosition.Get<0>(),AllPosition.Get<1>(),AllPosition.Get<2>());	
					Irene->NearMonsterAnalysis(HitMonsterList.Get<0>(), HitMonsterList.Get<1>(), HitMonsterList.Get<2>(), AllPosition.Get<0>().Z);
	
					if(Irene->IreneAttack->SwordTargetMonster != nullptr)
					{
						// 몬스터를 찾고 쳐다보기
						const float Z = UKismetMathLibrary::FindLookAtRotation(Irene->GetActorLocation(), Irene->IreneAttack->SwordTargetMonster->GetActorLocation()).Yaw;
						GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorRotation(FRotator(0.0f, Z, 0.0f));
					}
					else
					{
						const FVector IrenePosition = Irene->GetActorLocation();
						const float Z = UKismetMathLibrary::FindLookAtRotation(IrenePosition,IrenePosition + Irene->IreneInput->GetMoveKeyToDirVector()).Yaw;
						Irene->SetActorRotation(FRotator(0.0f, Z, 0.0f));
					}
					const FVector SpawnLocation = Irene->GetActorLocation() + (Irene->GetActorForwardVector() * 100);
					Irene->IreneSpirit = GetWorld()->SpawnActor<AIreneSpirit>(Irene->IreneSpiritOrigin, SpawnLocation, Irene->GetActorRotation());
					if(Irene->IreneSpirit == nullptr)
					{
						Irene->IreneSpirit = GetWorld()->SpawnActor<AIreneSpirit>(Irene->IreneSpiritOrigin, Irene->GetActorLocation(), Irene->GetActorRotation());
						if(Irene->IreneSpirit == nullptr)
							return;
					}
				}
			}
			
			if(Irene->bIsSpiritStance)
				SpiritSkill();

			Irene->IreneAttack->SetSkillState();
			
			if(!Irene->bIsSpiritStance)
				NonSpiritSkill();
			//Irene->IreneUIManager->PlayerHud->UseSkill();			
		}
	}
}
void UIreneInputInstance::NonSpiritSkill()
{
	const TUniquePtr<FAttackDataTable> AttackTable = MakeUnique<FAttackDataTable>(*Irene->IreneAttack->GetNameAtAttackDataTable(Irene->IreneAttack->GetBasicAttackDataTableName()));

	// 공격력 계산으로 기본적으로 ATTACK_DAMAGE_1만 사용함
	if(AttackTable != nullptr)
	{
		Irene->IreneData.Strength = AttackTable->ATTACK_DAMAGE_1;
		// 검 스킬 쿨타임 적용
		MaxSwordSkillCoolTime = AttackTable->ATTACK_DAMAGE_2;

		if (Irene->makeIngameWidget != nullptr)
		{
			if(AttackTable->Name == "Skill_1_1")
				Irene->FInGameBattle.Broadcast(4, MaxSwordSkillCoolTime + CanSwordSkill2Time);
			else
				Irene->FInGameBattle.Broadcast(4, MaxSwordSkillCoolTime);

			Irene->FInGameBattle.Broadcast(2, 0.0f);
		}
	}
	
	Irene->IreneAnim->PlaySkillAttackMontage();
	Irene->IreneData.IsAttacking = true;
	if(PerfectDodgeTimerHandle.IsValid())
	{
		Irene->IreneAnim->SetDodgeDir(0);
		Irene->FollowTargetPosition();
		PerfectDodgeAttackEnd();
	}
			
	// 검 스킬 2번 사용 조건
	if (Irene->IreneState->GetStateToString().Compare(FString("Sword_Skill_1")) == 0 && CanUseSecondSwordSkill == false)
	{
		CanUseSecondSwordSkill = true;
		// 특정 시간안에 다시 우클릭하면 스킬2로 넘길 수 있다.
		GetWorld()->GetTimerManager().SetTimer(SwordSkill2WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			CanUseSecondSwordSkill = false;
			GetWorld()->GetTimerManager().ClearTimer(SwordSkill2WaitHandle);
		}), CanSwordSkill2Time, false);
	}
			
	if(Irene->IreneState->GetStateToString().Compare(FString("Sword_Skill_1")) == 0)
	{
		// 검 스킬 1번 애니메이션이 끝나고 검 스킬 2번 사용 조건이 끝나면 쿨타임 시작
		GetWorld()->GetTimerManager().SetTimer(SwordSkillEndWaitHandle, this, &UIreneInputInstance::SwordSkillEndWait, CanSwordSkill2Time, false);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(SwordSkillEndWaitHandle);
		GetWorld()->GetTimerManager().ClearTimer(SwordSkillWaitHandle);
		SwordSkillCoolTime = 0.0f;
		// 검 스킬 2번 사용 즉시 쿨타임 시작
		SwordSkillEndWait();
	}
}
void UIreneInputInstance::SpiritSkill()
{
	const TUniquePtr<FAttackDataTable> AttackTable = MakeUnique<FAttackDataTable>(*Irene->IreneAttack->GetNameAtAttackDataTable(Irene->IreneAttack->GetBasicAttackDataTableName()));

	// 공격력 계산으로 기본적으로 ATTACK_DAMAGE_1만 사용함
	if(AttackTable != nullptr)
	{
		Irene->IreneData.Strength = AttackTable->ATTACK_DAMAGE_1;
	}

	// 잔상 다음 공격 애니메이션 실행 조건
	if (CanUseSpiritAnimJump == false)
	{
		CanUseSpiritAnimJump = true;
		// 특정 시간안에 다시 우클릭하면 다음 공격 애니로 넘길 수 있다.
		GetWorld()->GetTimerManager().SetTimer(SpiritAnimJumpWaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			CanUseSpiritAnimJump = false;
			SpiritChainAttackCount = 0;
			GetWorld()->GetTimerManager().ClearTimer(SpiritAnimJumpWaitHandle);
		}), SpiritAnimJumpCoolTime, false);
	}
	
	if(Irene->IreneSpirit != nullptr && CanUseSpiritAnimJump == true)
	{
		switch (SpiritChainAttackCount)
		{
		case 1:
			SpiritSpawnCoolTime = 0.17f;
			SpiritChainAttackCount = 2;
			break;
		case 2:
			SpiritSpawnCoolTime = 0.33f;
			SpiritChainAttackCount = 3;
			break;
		case 3:
			SpiritSpawnCoolTime = 0.54f;
			SpiritChainAttackCount = 1;
			break;
		default:
			break;
		}
	}

	if(SpiritChainAttackCount == 0)
	{
		SpiritSpawnCoolTime = 0.54f;
		SpiritChainAttackCount = 1;
	}
	
	Irene->IreneAnim->PlaySkillAttackMontage();
	if(Irene->IreneSpirit != nullptr)
		Irene->IreneSpirit->IreneSpiritAnim->PlaySkillAttackMontage(SpiritChainAttackCount);
	
	Irene->IreneData.IsAttacking = true;
	if(PerfectDodgeTimerHandle.IsValid())
	{
		Irene->IreneAnim->SetDodgeDir(0);
		Irene->FollowTargetPosition();
		PerfectDodgeAttackEnd();
	}
	
	if(Irene->GetMesh()->IsVisible())
		Irene->Weapon->SetVisibility(true);
	GetWorld()->GetTimerManager().SetTimer(SpiritSpawnWaitHandle,this, &UIreneInputInstance::SpawnSpirit, SpiritSpawnCoolTime, false);
}
void UIreneInputInstance::SpawnSpirit()
{
	if(Irene->IreneSpirit != nullptr)
	{
		//Irene->IreneSpirit->IreneSpiritAnim->PlaySkillAttackMontage(SpiritChainAttackCount);
	}
}
void UIreneInputInstance::SkillWait()
{
	// UI작성 시 주석 모두 제거
	SwordSkillCoolTime += CoolTimeRate;
	
	if (SwordSkillCoolTime > MaxSwordSkillCoolTime)
	{
		bIsSkillOn = true;
		//Irene->IreneUIManager->UpdateSkillCool(SwordSkillCoolTime, MaxSwordSkillCoolTime);
		//Irene->IreneUIManager->OnSkillCoolChange.Broadcast();
		SwordSkillCoolTime = 0.0f;
		GetWorld()->GetTimerManager().ClearTimer(SwordSkillWaitHandle);

		if (Irene->makeIngameWidget != nullptr)
		{
			Irene->FInGameBattle.Broadcast(3, 0.0f);
			Irene->FInGameBattle.Broadcast(3, 0.0f);
		}
	}
	//else
	//{
		//Irene->IreneUIManager->UpdateSkillCool(SwordSkillCoolTime, MaxSwordSkillCoolTime);
		//Irene->IreneUIManager->OnSkillCoolChange.Broadcast();
	//}
}
void UIreneInputInstance::SwordSkillEndWait()
{
	GetWorld()->GetTimerManager().SetTimer(SwordSkillWaitHandle, this, &UIreneInputInstance::SkillWait, CoolTimeRate, true);
}
#pragma endregion Skill

void UIreneInputInstance::MouseWheel(float Rate)
{
	if (!Irene->IreneState->IsDeathState() && !Irene->bInputStop)
	{
		// 줌인줌아웃
		Irene->SpringArmComp->TargetArmLength -= Rate * Irene->IreneData.MouseWheelSpeed;

		Irene->STGameInstance->GetPlayerBattleState() == true ?
			Irene->SpringArmComp->TargetArmLength = FMath::Clamp(Irene->SpringArmComp->TargetArmLength, Irene->IreneData.MinFollowCameraZPosition, Irene->IreneData.BattleCameraZPosition) :
			Irene->SpringArmComp->TargetArmLength = FMath::Clamp(Irene->SpringArmComp->TargetArmLength, Irene->IreneData.MinFollowCameraZPosition, Irene->IreneData.MaxFollowCameraZPosition);
	}
}

#pragma region Dodge
void UIreneInputInstance::DodgeKeyword()
{
	// 브레이킹어택 중 타겟몬스터 6스택 미만일 때 회피 가능처리
	if(Irene->bInputStop && !Irene->IreneState->IsDeathState() && Irene->IreneAnim->GetIsMoveStopBreakAttack())
	{
		Irene->bInputStop = false;
	}
	
	if (!Irene->GetMovementComponent()->IsFalling() && !Irene->IreneState->IsDeathState() && !DodgeInputWaitHandle.IsValid() && !PerfectDodgeTimerHandle.IsValid() &&
		//Irene->IreneState->GetStateToString().Compare(FString("Dodge_Start"))!=0 &&
		(Irene->IreneAttack->GetCanDodgeJumpSkip()||!Irene->IreneState->IsAttackState()) && (Irene->IreneAttack->GetCanDodgeJumpSkip()||!Irene->IreneState->IsSkillState()) &&
		bIsDodgeOn && !bIsDialogOn && !Irene->bInputStop && !bIsStun)
	{
		// 잔상 공격 중 회피
		if(Irene->IreneSpirit != nullptr)
		{
			Irene->IreneSpirit->DestroySpirit();
			Irene->IreneSpirit = nullptr;
			Irene->GetMesh()->SetVisibility(true);
			Irene->Weapon->SetVisibility(false);
		}
		
		// 회피 중 회피
		if(Irene->IreneState->GetStateToString().Compare(FString("Dodge_Start"))==0 && bIsDodgeToDodge)
		{
			Irene->ChangeStateAndLog(UIdleState::GetInstance());
			GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([&]{DodgeKeyword();}));
			bIsDodgeToDodge = false;
			return;
		}
		
		Irene->ChangeStateAndLog(UDodgeStartState::GetInstance());
		
		Irene->PetAnim->SetHeliosStateAnim(EHeliosStateEnum::DodgeStart);
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([&]{Irene->PetAnim->SetHeliosStateAnim(EHeliosStateEnum::Idle);}));

		RecoveryDodge();
		
		Irene->IreneAnim->SetIsStartBreakAttack(false);
		Irene->IreneAnim->SetIsMoveStopBreakAttack(false);
		Irene->IreneAnim->SetIsDoAttackBreakAttack(false);
		Irene->IreneAnim->StopAllMontages(0);
		
		Irene->SetActorRelativeRotation(GetMoveKeyToDirVector().Rotation());
		Irene->IreneData.IsInvincibility = true;

		if(Irene->IreneAttack->GetIsPerfectDodge() && Irene->IreneAttack->PerfectDodgeMonster != nullptr)
			PerfectDodge();
		
		const TUniquePtr<FAttackDataTable> AttackTable = MakeUnique<FAttackDataTable>(*Irene->IreneAttack->GetNameAtAttackDataTable(Irene->IreneAttack->GetBasicAttackDataTableName()));

		constexpr float InvincibilityTime = 0.3f;
		GetWorld()->GetTimerManager().SetTimer(DodgeInvincibilityTimerHandle, FTimerDelegate::CreateLambda([&]()
		 {
			if(!PerfectDodgeInvincibilityTimerHandle.IsValid())
			{
				Irene->IreneData.IsInvincibility = false;
			}
			GetWorld()->GetTimerManager().ClearTimer(DodgeInvincibilityTimerHandle);
		 }), InvincibilityTime, false);
		
		GetWorld()->GetTimerManager().SetTimer(DodgeInputWaitHandle, FTimerDelegate::CreateLambda([&]()
		 {
			GetWorld()->GetTimerManager().ClearTimer(DodgeInputWaitHandle);
		 }), AttackTable->C_Time*Irene->CustomTimeDilation, false);
	}
}
void UIreneInputInstance::PerfectDodge()
{
	if(MoveKey[0] == 0 && MoveKey[1] == 0 && MoveKey[2] == 0 && MoveKey[3] == 0)
	{
		const float Z = UKismetMathLibrary::FindLookAtRotation(Irene->GetActorLocation(), Irene->IreneAttack->PerfectDodgeMonster->GetActorLocation()).Yaw;
		Irene->SetActorRotation(FRotator(0.0f, Z+180, 0.0f));
	}
	constexpr float Time = 2.5f;
	constexpr float InvincibilityTime = 1.0f;
	GetWorld()->GetTimerManager().SetTimer(PerfectDodgeInvincibilityTimerHandle, FTimerDelegate::CreateLambda([&]()
	 {
		Irene->IreneData.IsInvincibility = false;
		GetWorld()->GetTimerManager().ClearTimer(PerfectDodgeInvincibilityTimerHandle);
	 }), InvincibilityTime * Irene->CustomTimeDilation, false);
	
	const TUniquePtr<FWeaponGauge> DataTable = MakeUnique<FWeaponGauge>(*Irene->IreneAttack->GetNameAtWeaponGaugeDataTable(FName("Perfect_Dodge")));
	Irene->IreneAttack->SetGauge(DataTable->Get_W_Gauge);
	PerfectDodgeStart();
	Irene->IreneAnim->SetDodgeDir(10);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(),SlowScale);
	Irene->CustomTimeDilation = 1/SlowScale;
}

void UIreneInputInstance::PerfectDodgeStart()
{
	Irene->PerfectDodgeStart();
}
void UIreneInputInstance::PerfectDodgeTimeEnd()
{
	Irene->PerfectDodgeTimeEnd();
}
void UIreneInputInstance::PerfectDodgeAttackEnd()
{
	Irene->PerfectDodgeAttackEnd();
}
void UIreneInputInstance::PerfectDodgePlayOver()
{
	if(bPerfectDodgeToAttack)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(),1);
		Irene->CustomTimeDilation = 1;
		GetWorld()->GetTimerManager().ClearTimer(PerfectDodgeTimerHandle);
		
		Irene->FollowTargetPosition();
		bPerfectDodgeToAttack = false;
		Irene->IreneAnim->SetDodgeDir(0);
		PerfectDodgeAttackEnd();
		GetWorld()->GetTimerManager().ClearTimer(AttackWaitHandle);
		Irene->ChangeStateAndLog(UIdleState::GetInstance());
		LeftButton(1.0f);
	}
	else
	{
		Irene->CustomTimeDilation = 1;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(),1);
		Irene->IreneAttack->SetIsPerfectDodge(false);
		Irene->IreneAnim->SetDodgeDir(0);
		SetStopMoveAutoTarget();
		PerfectDodgeTimeEnd();
	}	
}

void UIreneInputInstance::RecoveryDodge()
{
	// UI작성 시 주석 모두 제거
	if(bIsDodgeOn)
	{
		if(DodgeCount > 0)
		{
			const TUniquePtr<FAttackDataTable> AttackTable = MakeUnique<FAttackDataTable>(*Irene->IreneAttack->GetNameAtAttackDataTable(Irene->IreneAttack->GetBasicAttackDataTableName()));
			MaxDodgeCoolTime = AttackTable->Attack_Distance_2;

			if(Irene->bIsSpiritStance)
				MaxDodgeCoolTime = 0;
			
			if (AttackTable != nullptr)
			{
				DodgeCount--;
				if (DodgeCount != 0)
				{
					if(!DodgeWaitHandle.IsValid())
						GetWorld()->GetTimerManager().SetTimer(DodgeWaitHandle, this, &UIreneInputInstance::RecoveryDodgeWait, CoolTimeRate, true, 0.0f);
				}
				else
				{
					bIsDodgeOn = false;
				}
			}
			//Irene->IreneUIManager->PlayerHud->UseSkill();
		}
	}
}
void UIreneInputInstance::RecoveryDodgeWait()
{
	DodgeCoolTime += CoolTimeRate;
	if (DodgeCoolTime > MaxDodgeCoolTime)
	{
		bIsDodgeOn = true;
		//Irene->IreneUIManager->UpdateThunderSkillCool(SpearSkill1CoolTime, MaxSpearSkill1CoolTime);
		//Irene->IreneUIManager->OnThunderSkillCoolChange.Broadcast();
		DodgeCoolTime = 0.0f;
		if (DodgeCount < MaxDodgeCount)
		{
			DodgeCount++;
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(DodgeWaitHandle);
		}
	}
	else
	{
		//Irene->IreneUIManager->UpdateThunderSkillCool(SpearSkill1CoolTime, MaxSpearSkill1CoolTime);
		//Irene->IreneUIManager->OnThunderSkillCoolChange.Broadcast();
	}
}
#pragma endregion Dodge
	
#pragma region Spirit
void UIreneInputInstance::SpiritChangeKeyword()
{
	if(!Irene->bInputStop)
	{
		if(Irene->IreneData.CurrentGauge >= 0 && bIsSpiritChangeEnable)
		{
			if(!Irene->bIsSpiritStance && Irene->IreneData.CurrentGauge > 0)
			{
				// 정령 스탠스 적용
				Irene->SpiritStance();
				GetWorld()->GetTimerManager().ClearTimer(SwordSkillWaitHandle);
				Irene->PetAnim->SetHeliosStateAnim(EHeliosStateEnum::FormChange);

				if (auto INGAME = Cast<UIngameWidget_D>(Irene->makeIngameWidget))
					INGAME->STANCEGAUGEeCtime(-2.0f);

				//Irene->IreneData.CurrentGauge = 0;
				Irene->IreneUIManager->UpdateSoul(Irene->IreneData.CurrentGauge, Irene->IreneData.MaxGauge);
				//GetWorld()->GetTimerManager().SetTimer(WeaponChangeWaitHandle,this, &UIreneInputInstance::SpiritChangeTimeOver, 60, false);
				//GetWorld()->GetTimerManager().SetTimer(WeaponChangeMaxWaitHandle,this, &UIreneInputInstance::SpiritChangeMaxTime, 80, false);				
				Irene->IreneAnim->StopAllMontages(0);
				Irene->IreneAnim->SetSpiritStart(true);
				Irene->ChangeStateAndLog(UFormChangeState::GetInstance());
				GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([&]
				{
					Irene->IreneAnim->SetSpiritStart(false);
					Irene->PetAnim->SetHeliosStateAnim(EHeliosStateEnum::Idle);
				}));
				Irene->bIsSpiritStance = true;
				bIsSpiritChangeEnable = false;
				GetWorld()->GetTimerManager().SetTimer(NormalToSpiritWaitHandle,[&]
				{
					Irene->PetMesh->SetVisibility(false,true);
					bIsSpiritChangeEnable = true;
				} , 1.5f, false);
			}
			else if(Irene->bIsSpiritStance)
			{
				// 정령 스탠스 해제
				Irene->NormalStance();

				GetWorld()->GetTimerManager().ClearTimer(SwordSkillWaitHandle);

				GetWorld()->GetTimerManager().ClearTimer(WeaponChangeWaitHandle);
				GetWorld()->GetTimerManager().ClearTimer(WeaponChangeMaxWaitHandle);
				
				GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([&]{Irene->ActionEndChangeMoveState();}));
				
				if (auto INGAME = Cast<UIngameWidget_D>(Irene->makeIngameWidget))
					INGAME->STANCEGAUGEeCtime(-1.0f);

				Irene->IreneAnim->StopAllMontages(0);
				Irene->ActionEndChangeMoveState();
				Irene->bIsSpiritStance = false;
				Irene->PetMesh->SetVisibility(true,true); 
				Irene->PetAnim->SetHeliosStateAnim(EHeliosStateEnum::FormChangeNormal);

				bIsSpiritChangeEnable = false;
				GetWorld()->GetTimerManager().SetTimer(NormalToSpiritWaitHandle,[&]
				{
					bIsSpiritChangeEnable = true;
				} , 1.33f, false);
				
				const auto Instance = Cast<USTGameInstance>(Irene->GetGameInstance());
				if (Instance != nullptr)
				{
					Instance->ExplodeCurStackMonster();
				}
			}
		}
	}
}
void UIreneInputInstance::SpiritChangeTimeOver()
{
	if(Irene->bIsSpiritStance)
	{
		SpiritTimeOverDeBuff();
		GetWorld()->GetTimerManager().SetTimer(SpiritTimeDamageOverTimer,this, &UIreneInputInstance::SpiritTimeOverDeBuff, 0.2, true);
		GetWorld()->GetTimerManager().ClearTimer(WeaponChangeWaitHandle);
	}
}
void UIreneInputInstance::SpiritChangeMaxTime()
{
	if(Irene->bIsSpiritStance)
	{
		GetWorld()->GetTimerManager().ClearTimer(WeaponChangeWaitHandle);
		GetWorld()->GetTimerManager().ClearTimer(SpiritTimeDamageOverTimer);
		bIsStun = true;

		MoveKey[0] = 0;
		MoveKey[1] = 0;
		MoveKey[2] = 0;
		MoveKey[3] = 0;

		GetWorld()->GetTimerManager().SetTimer(SpiritTimeStunOverTimer,FTimerDelegate::CreateLambda([&]
		{
			bIsStun = false;
			Irene->PetAnim->SetHeliosStateAnim(EHeliosStateEnum::Idle);
		}), 3, false);
		bIsSpiritChangeEnable = true;
		SpiritChangeKeyword();
		Irene->PetAnim->SetHeliosStateAnim(EHeliosStateEnum::Stun);
	}
}
void UIreneInputInstance::SpiritTimeOverDeBuff()
{
	if(!Irene->bIsSpiritStance || Irene->IreneState->IsDeathState())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpiritTimeDamageOverTimer);
		return;
	}
	Irene->SetHP(100);
}
void UIreneInputInstance::SpiritChangeBlock()
{
	// 정령 스탠스 해제
	GetWorld()->GetTimerManager().ClearTimer(SwordSkillWaitHandle);

	GetWorld()->GetTimerManager().ClearTimer(WeaponChangeWaitHandle);
	GetWorld()->GetTimerManager().ClearTimer(WeaponChangeMaxWaitHandle);

	Irene->IreneAnim->StopAllMontages(0);
	Irene->IreneAttack->AttackTimeEndState();
	Irene->bIsSpiritStance = false;
	Irene->PetMesh->SetVisibility(true,true);

	const auto Instance = Cast<USTGameInstance>(Irene->GetGameInstance());
	if (Instance != nullptr)
	{
		Instance->InitCurStackMonster();
	}

	bIsSpiritChangeEnable = false;
}
#pragma endregion Spirit

#pragma region BreakAttack
void UIreneInputInstance::BreakAttackKeyword()
{
	if(BreakAttackSpirit == nullptr && !BreakAttackWaitHandle.IsValid() && Irene->bIsSpiritStance)
	{
		Irene->IreneAnim->StopAllMontages(0.0f);
		
		const TUniquePtr<FAttackDataTable> AttackTable = MakeUnique<FAttackDataTable>(*Irene->IreneAttack->GetNameAtAttackDataTable("S_Dodge"));

		// 카메라 위치를 기반으로 박스를 만들어서 몬스터들을 탐지하는 방법	
		auto AllPosition = Irene->SetCameraStartTargetPosition(FVector(10,100,1200),Irene->CameraComp->GetComponentLocation());
		auto HitMonsterList = Irene->StartPositionFindNearMonster(AllPosition.Get<0>(),AllPosition.Get<1>(),AllPosition.Get<2>());	

		if(!HitMonsterList.Get<2>())
			return;
		
		if(Irene->IreneAttack->SwordTargetMonster != nullptr)
		{
			const auto Mon=Cast<AMonster>(Irene->IreneAttack->SwordTargetMonster);
			Mon->MarkerOff();
			Irene->IreneAnim->SetIsHaveTargetMonster(false);
			Irene->IreneAnim->SetTargetMonster(nullptr);
			Irene->IreneAttack->SwordTargetMonster = nullptr;
		}		
		
		Irene->NearMonsterAnalysis(HitMonsterList.Get<0>(), HitMonsterList.Get<1>(), HitMonsterList.Get<2>(), AllPosition.Get<0>().Z);
		
		GetWorld()->GetTimerManager().SetTimer(BreakAttackWaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			GetWorld()->GetTimerManager().ClearTimer(BreakAttackWaitHandle);
		}), AttackTable->C_Time, false);
		
		if(Irene->IreneAttack->SwordTargetMonster == nullptr)
			return;

		Irene->bInputStop = true;
		bCameraStop = true;
		Irene->IreneAnim->SetIsStartBreakAttack(true);
		
		if(Irene->IreneAttack->SwordTargetMonster != nullptr)
		{
			// 몬스터를 찾고 쳐다보기
			const float Z = UKismetMathLibrary::FindLookAtRotation(Irene->GetActorLocation(), Irene->IreneAttack->SwordTargetMonster->GetActorLocation()).Yaw;
			GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorRotation(FRotator(0.0f, Z, 0.0f));

			const float Y = Irene->WorldController->GetControlRotation().Pitch;
			// 카메라 원점 조정
			FRotator Rotator = FRotator::ZeroRotator;
			Rotator.Pitch = Y;
			Rotator.Yaw = Z;
			Irene->WorldController->SetControlRotation(Rotator);
		}
		
		const FVector SpawnLocation = Irene->GetActorLocation();
		BreakAttackSpirit = GetWorld()->SpawnActor<AIreneSpirit>(Irene->IreneSpiritOrigin, SpawnLocation, Irene->GetActorRotation());
		BreakAttackSpirit->GetMesh()->SetVisibility(false,true);
		
		GetWorld()->GetTimerManager().SetTimer(BreakAttackFirstAnimTimer,this,&UIreneInputInstance::BreakAttackFirst, 0.17f, false);
		GetWorld()->GetTimerManager().SetTimer(BreakAttackAttackTimer,this,&UIreneInputInstance::BreakAttackSendAttack, 0.01f, false);
		GetWorld()->GetTimerManager().SetTimer(BreakAttackCameraStopTimer,[&]
		{
			bCameraStop = false;
			Irene->SpringArmComp->CameraLagSpeed = 10.0f;
		}, 0.4f, false);
		GetWorld()->GetTimerManager().SetTimer(BreakAttackSpiritTimer,this,&UIreneInputInstance::BreakAttackEnd, 0.4f, false);
	}
}
void UIreneInputInstance::BreakAttackFirst()
{
	Irene->GetMesh()->SetVisibility(false,true);
	BreakAttackSpirit->GetMesh()->SetVisibility(true,true);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BreakAttackStartEffect, Irene->GetActorLocation());
	
	if(Irene->IreneAttack->SwordTargetMonster == nullptr)
		return;

	const auto Mon = Cast<AMonster>(Irene->IreneAttack->SwordTargetMonster);
	const float Dist = FVector::Dist(Irene->IreneAttack->SwordTargetMonster->GetActorLocation(), Irene->GetActorLocation());
	const float Collision = Irene->GetCapsuleComponent()->GetScaledCapsuleRadius() + Mon->GetCapsuleComponent()->GetScaledCapsuleRadius();
	Irene->SetActorLocation(Irene->GetActorLocation()+Irene->GetActorForwardVector()*(Dist-Collision));
}
void UIreneInputInstance::BreakAttackSendAttack()
{
	if(Irene->IreneAttack->SwordTargetMonster == nullptr)
		return;
	
	const auto Mon = Cast<AMonster>(Irene->IreneAttack->SwordTargetMonster);
	if(Mon->GetCurStackCount() == 6)
	{
		BreakAttackWaitTime = 0.4f;
		Irene->IreneAnim->SetIsDoAttackBreakAttack(true);
		Mon->StackExplode();
	}
	else
	{
		BreakAttackWaitTime = 0.3f;
		Irene->IreneAnim->SetIsMoveStopBreakAttack(true);
	}
}
void UIreneInputInstance::BreakAttackEnd()
{	
	GetWorld()->GetTimerManager().SetTimer(BreakAttackAnimTimer,[&]
	{
		Irene->IreneAnim->SetIsMoveStopBreakAttack(false);
		Irene->IreneAnim->SetIsDoAttackBreakAttack(false);
		Irene->bInputStop = false;
	}, BreakAttackWaitTime, false);
	
	Irene->IreneAnim->SetIsStartBreakAttack(false);
	BreakAttackSpirit->Destroy();
	BreakAttackSpirit = nullptr;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BreakAttackEndEffect, Irene->GetActorLocation());
	Irene->GetMesh()->SetVisibility(true,true);
}
#pragma endregion BreakAttack

#pragma region UltimateAttack
void UIreneInputInstance::UltimateAttackKeyword()
{
	//Irene->bIsSpiritStance && Irene->IreneData.CurrentUltimateAttackGauge == Irene->IreneData.MaxUltimateAttackGauge &&
	if(!Irene->bInputStop && !Irene->IreneState->IsUltimateAttackState())
	{
		// 선딜
		// 카메라 위치를 기반으로 박스를 만들어서 몬스터들을 탐지하는 방법	
		auto AllPosition = Irene->SetCameraStartTargetPosition(FVector(10,100,1200),Irene->CameraComp->GetComponentLocation());
		auto HitMonsterList = Irene->StartPositionFindNearMonster(AllPosition.Get<0>(),AllPosition.Get<1>(),AllPosition.Get<2>());	

		if(!HitMonsterList.Get<2>())
			return;

		if(Irene->IreneAttack->SwordTargetMonster != nullptr)
		{
			const auto Mon=Cast<AMonster>(Irene->IreneAttack->SwordTargetMonster);
			Mon->MarkerOff();
			Irene->IreneAnim->SetIsHaveTargetMonster(false);
			Irene->IreneAnim->SetTargetMonster(nullptr);
			Irene->IreneAttack->SwordTargetMonster = nullptr;
		}
		
		Irene->NearMonsterAnalysis(HitMonsterList.Get<0>(), HitMonsterList.Get<1>(), HitMonsterList.Get<2>(), AllPosition.Get<0>().Z);	
		
		if(Irene->IreneAttack->SwordTargetMonster != nullptr)
		{
			Irene->IreneData.CurrentUltimateAttackGauge = 0;
			Irene->ChangeStateAndLog(UUltimateAttack::GetInstance());
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(),0.1f);
			Irene->CustomTimeDilation = 1/0.1f;
			Irene->IreneData.IsInvincibility = true;
			Irene->CameraComp->PostProcessSettings.bOverride_MotionBlurMax = true;
			Irene->CameraComp->PostProcessSettings.MotionBlurMax = 0;
			
			SkillCameraMoveStart();
			LevelSequencePlayer->Play();			
		}
	}
}
void UIreneInputInstance::UltimateAttack()
{
	// 쉴드
	Irene->ShieldParticleSystemComponent->Activate(true);
	Irene->IreneData.CurrentShield = Irene->IreneData.MaxShield;
	Irene->PetAnim->SetShield(true);

	// 공격
	// 카메라 위치를 기반으로 박스를 만들어서 몬스터들을 탐지하는 방법
	auto AllPosition = Irene->SetCameraStartTargetPosition(FVector(500,100,1200),Irene->CameraComp->GetComponentLocation());
	auto HitMonsterList = Irene->StartPositionFindNearMonster(AllPosition.Get<0>(),AllPosition.Get<1>(),AllPosition.Get<2>());	

	if(!HitMonsterList.Get<2>())
		return;
		
	if(Irene->IreneAttack->SwordTargetMonster != nullptr)
	{
		const auto Mon=Cast<AMonster>(Irene->IreneAttack->SwordTargetMonster);
		Mon->MarkerOff();
		Irene->IreneAnim->SetIsHaveTargetMonster(false);
		Irene->IreneAnim->SetTargetMonster(nullptr);
		Irene->IreneAttack->SwordTargetMonster = nullptr;
	}

	Irene->NearMonsterAnalysis(HitMonsterList.Get<0>(), HitMonsterList.Get<1>(), HitMonsterList.Get<2>(), AllPosition.Get<0>().Z,true);
}
void UIreneInputInstance::SkillCameraMoveStart()
{
	if(Irene->IreneAttack->SwordTargetMonster != nullptr)
	{
		const FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Irene->GetActorLocation(), Irene->IreneAttack->SwordTargetMonster->GetActorLocation());
		Irene->SetActorRotation(FRotator(0.0f, Rotator.Yaw, 0.0f));
		
		bSkillCameraMove = true;
		SkillCameraPlayTime = 0;
		SkillCameraEndPlayTime = 0;
		Irene->bInputStop = true;
	}	
}
void UIreneInputInstance::SkillCameraMoveEnd()
{
	bSkillCameraMove = false;
	SkillCameraPlayTime = 0;
	SkillCameraEndPlayTime = 0;
	Irene->bInputStop = false;

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(),1);
	Irene->CustomTimeDilation = 1;
	Irene->CameraComp->PostProcessSettings.bOverride_MotionBlurMax = false;
	Irene->CameraComp->PostProcessSettings.MotionBlurMax = 5.0f;

	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([&]{UltimateAttackSetCamera();}));
}
void UIreneInputInstance::UltimateAttackSetCamera()
{
	// 카메라
	if(Irene->IreneAttack->SwordTargetMonster != nullptr)
	{
		const FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Irene->GetActorLocation(), Irene->IreneAttack->SwordTargetMonster->GetActorLocation());
		Irene->SetActorRotation(FRotator(0.0f, Rotator.Yaw, 0.0f));
		// 카메라 원점 조정
		FRotator CameraRotator = FRotator::ZeroRotator;
		CameraRotator.Pitch = Rotator.Pitch;
		CameraRotator.Yaw = Rotator.Yaw;
		Irene->WorldController->SetControlRotation(CameraRotator);
		
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([&]{UltimateAttack();}));
	}	
}
#pragma endregion UltimateAttack

void UIreneInputInstance::DialogAction()
{
	UPlayerHudWidget* PlayerHud = Irene->IreneUIManager->PlayerHud;

	if (PlayerHud->GetDialogState() == EDialogState::e_Set)
	{
		PlayerHud->PlayDialog();
	}
}

void UIreneInputInstance::DialogPlaying()
{
	UPlayerHudWidget* PlayerHud = Irene->IreneUIManager->PlayerHud;

	switch (PlayerHud->GetDialogState())
	{
	case EDialogState::e_Playing:
		PlayerHud->PassDialog();
		break;
	case EDialogState::e_Complete:
		if (PlayerHud->ContinueDialog())
			PlayerHud->PlayDialog();
		else
			PlayerHud->ExitDialog();
		break;
	case EDialogState::e_Disable:
		break;
	default:
		break;
	}
}

void UIreneInputInstance::DialogSkip()
{
	if (bIsDialogOn)
	{
		UPlayerHudWidget* PlayerHud = Irene->IreneUIManager->PlayerHud;
		PlayerHud->SkipDialog();
	}
}

void UIreneInputInstance::MouseCursorKeyword()
{
	if (!Irene->IreneState->IsDeathState() && !bIsDialogOn && !Irene->bInputStop)
	{
		// 마우스 커서 숨기거나 보이게 하는 함수
		if (Irene->WorldController->bShowMouseCursor == false)
			Irene->WorldController->bShowMouseCursor = true;
		else
			Irene->WorldController->bShowMouseCursor = false;
	}
}
#pragma endregion Input

#pragma region UI
void UIreneInputInstance::PauseWidgetOn()
{
	if (Irene->IreneUIManager->GetIsPauseOnScreen())
	  	Irene->IreneUIManager->PauseWidgetOff();
	else
	{
		Irene->IreneState->SetState(UIdleState::GetInstance());
		Irene->IreneUIManager->PauseWidgetOn();
	}
	Irene->ActionEndChangeMoveState();
}
#pragma endregion UI

#pragma region CheckStateChange
bool UIreneInputInstance::CanRunState() const
{
	if (!Irene->IreneState->IsAttackState() && !Irene->IreneState->IsJumpState() && !Irene->IreneState->IsDeathState())
			return true;
	return false;
}
bool UIreneInputInstance::CanAttackState() const
{
	if(Irene->IreneState->GetStateToString().Compare(FString("Dodge_End"))==0)
		return true;
	if (!Irene->IreneState->IsJumpState() && !Irene->IreneState->IsFormChangeState() && (!Irene->IreneState->IsDodgeState() || Irene->IreneAnim->GetDodgeDir() == 10) && !Irene->IreneState->IsSkillState() && !Irene->IreneState->IsDeathState())
		return true;
	return false;
}
bool UIreneInputInstance::CanSkillState() const
{
	if(Irene->IreneState->GetStateToString().Compare(FString("Dodge_End"))==0)
		return true;
	if (!Irene->IreneState->IsJumpState() && !Irene->IreneState->IsFormChangeState() &&  !Irene->IreneState->IsDodgeState() && !Irene->IreneState->IsAttackState() && !Irene->IreneState->IsSkillState() && !Irene->IreneState->IsDeathState())
		return true;
	return false;
}
#pragma endregion CheckStateChange

#pragma region GetSet
void UIreneInputInstance::SetStartMoveAutoTarget(const FVector SetPlayerPosVec, const FVector SetTargetPosVec)const
{
	Irene->IreneAttack->SetFollowTarget(true);
	Irene->IreneAttack->SetFollowTargetAlpha(0);
	Irene->IreneAttack->SetPlayerPosVec(SetPlayerPosVec);
	Irene->IreneAttack->SetTargetPosVec(SetTargetPosVec);	
}
void UIreneInputInstance::SetStopMoveAutoTarget()const
{
	Irene->IreneAttack->SetFollowTarget(false);
	Irene->IreneAttack->SetFollowTargetAlpha(0);
	Irene->IreneAttack->SetPlayerPosVec(FVector::ZeroVector);
	Irene->IreneAttack->SetTargetPosVec(FVector::ZeroVector);	
}
#pragma endregion GetSet
