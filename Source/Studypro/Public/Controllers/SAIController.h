// SAIController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SAIController.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API ASAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASAIController();

	void BeginAI(APawn* InPawn);	//BT

	void EndAI();	//BT


protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void OnPatrolTimerElapsed();

public:
	//FTimerHandle PatrolTimerHandle = FTimerHandle();

	//static const float PatrolRepeatInterval;

	static const float PatrolRadius;

	static const FName StartPatrolPositionKey;

	static const FName EndPatrolPositionKey;

	static const FName TargetActorKey;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASAIController", Meta = (AllowprivateAccess))
	TObjectPtr<class UBlackboardData> BlackboardDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASAIController", Meta = (AllowprivateAccess))
	TObjectPtr<class UBehaviorTree> BehaviorTree;

};
