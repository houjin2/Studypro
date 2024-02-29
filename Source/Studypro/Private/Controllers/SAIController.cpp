// SAIController.cpp


#include "Controllers/SAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

const float ASAIController::PatrolRadius(5000.f);
const FName ASAIController::StartPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName ASAIController::EndPatrolPositionKey(TEXT("EndpatrolPosition"));
const FName ASAIController::TargetActorKey(TEXT("TargetActor"));


ASAIController::ASAIController()
{
    Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
    BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));    //BrainComponent�� AIController Ŭ������ ���ǵ� �Ӽ�. UBrainComponent <- UBehaviorTreeComponent�� ��������
}

void ASAIController::BeginAI(APawn* InPawn)
{
    UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
    if (true == ::IsValid(BlackboardComponent))
    {
        if (true == UseBlackboard(BlackboardDataAsset, BlackboardComponent))
        {
            bool bRunSucceeded = RunBehaviorTree(BehaviorTree);
            ensure(true == bRunSucceeded);
            UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("BeginAI() has been called.")));

            BlackboardComponent->SetValueAsVector(StartPatrolPositionKey, InPawn->GetActorLocation());
        }
    }
}

void ASAIController::EndAI()
{
    UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
    if (true == ::IsValid(BehaviorTreeComponent))
    {
        BehaviorTreeComponent->StopTree();
        UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("EndAI() has been called.")));
    }
}

void ASAIController::BeginPlay()
{
    Super::BeginPlay();

    APawn* ControllerPawn = GetPawn();
    if (true == ::IsValid(ControllerPawn))
    {
        BeginAI(ControllerPawn);
    }
}


void ASAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    EndAI();
}

void ASAIController::OnPatrolTimerElapsed()
{
    APawn* ControlledPawn = GetPawn();
    if (true == ::IsValid(ControlledPawn))
    {
        UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
        if (true == ::IsValid(NavSystem))
        {
            FNavLocation NextLocation;
            if (true == NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, PatrolRadius, NextLocation))
            {
                UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
            }
        }
    }
}
