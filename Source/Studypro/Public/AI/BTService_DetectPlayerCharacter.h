// BTService_DetectPlayerCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DetectPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API UBTService_DetectPlayerCharacter : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_DetectPlayerCharacter();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodemMemory, float DeltaSeconds) override;
};
