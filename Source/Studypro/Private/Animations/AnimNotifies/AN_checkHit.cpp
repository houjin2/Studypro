// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/AN_checkHit.h"
#include "Characters/SRPGCharacter.h"

void UAN_checkHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (true == ::IsValid(MeshComp))
	{
		ASRPGCharacter* AttackingCharacter = Cast<ASRPGCharacter>(MeshComp->GetOwner());
		if (true == ::IsValid(AttackingCharacter))
		{
			AttackingCharacter->CheckHit();
		}
	}
}
