// BTTask_Attack.cpp


#include "AI/BTTask_Attack.h"
#include "Controllers/SAIController.h"
#include "Characters/SNonPlayerCharacter.h"


UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ASAIController* AIC = Cast<ASAIController>(OwnerComp.GetOwner());
	if (true == ::IsValid(AIC))
	{
		ASNonPlayerCharacter* NPC = Cast<ASNonPlayerCharacter>(AIC->GetPawn());
		if(true == ::IsValid(NPC))
		{
			if (false == NPC->IsNowAttacking())
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);	//공격 Task는 지연 Task라 FinishLatentTask()를 호출하지 않으면 계속 머물게됨
			}
		}
	}
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ASAIController* AIC = Cast<ASAIController>(OwnerComp.GetOwner());
	if (true == ::IsValid(AIC))
	{
		ASNonPlayerCharacter* NPC = Cast<ASNonPlayerCharacter>(AIC->GetPawn());
		if (true == ::IsValid(NPC))
		{
			NPC->Attack();
		}
	}
	return EBTNodeResult::InProgress;
}


