// AN_checkHit.h

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_checkHit.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API UAN_checkHit : public UAnimNotify
{
	GENERATED_BODY()
	

private:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
};
