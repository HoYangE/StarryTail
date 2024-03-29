// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeJumpSkipNotifyState.h"

#include "../IreneCharacter.h"
#include "../PlayerSpirit/IreneSpirit.h"

UDodgeJumpSkipNotifyState::UDodgeJumpSkipNotifyState()
{
	bIsNativeBranchingPoint = true;
}

void UDodgeJumpSkipNotifyState::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);

	const auto Irene = BranchingPointPayload.SkelMeshComponent->GetOwner<AIreneCharacter>();
	if(Irene != nullptr)
	{
		Irene->IreneAttack->SetCanDodgeJumpSkip(true);
	}
}
void UDodgeJumpSkipNotifyState::BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime)
{
	Super::BranchingPointNotifyTick(BranchingPointPayload, FrameDeltaTime);
	
}
void UDodgeJumpSkipNotifyState::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);
	const auto Irene = BranchingPointPayload.SkelMeshComponent->GetOwner<AIreneCharacter>();
	if(Irene != nullptr)
	{
		Irene->IreneAttack->SetCanDodgeJumpSkip(false);
	}
}

