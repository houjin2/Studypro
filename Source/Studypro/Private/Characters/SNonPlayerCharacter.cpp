// SNonPlayerCharacter.cpp


#include "Characters/SNonPlayerCharacter.h"
#include "Controllers/SAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animations/SAnimInstance.h"
#include "Characters//SRPGCharacter.h"
#include "Components/CapsuleComponent.h"

ASNonPlayerCharacter::ASNonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

    AIControllerClass = ASAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASNonPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;   //캐릭터 이동방향에 회전을 일치시키기
    GetCharacterMovement()->bUseControllerDesiredRotation = true;   //컨트롤로테이션을 목표 회전으로 삼고 지정한 속도로 돌리기. 위에 것을 false로 하고 이걸 true로 한다면 마우스 방향에 따라 고개가 바로 돌아감
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

    GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

float ASNonPlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float FinalDamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);


    CurrentHP = FMath::Clamp(CurrentHP - FinalDamageAmount, 0.f, MaxHP);
    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s [%.1f, %.1f]"), *GetName(), CurrentHP, MaxHP));

    if (CurrentHP < KINDA_SMALL_NUMBER)
    {
        bIsDead = true;
        CurrentHP = 0.f;
        ASRPGCharacter* DamageCauserCharacter = Cast<ASRPGCharacter>(DamageCauser);
        if (true == ::IsValid(DamageCauserCharacter))
        {
            DamageCauserCharacter->SetCurrentEXP(DamageCauserCharacter->GetCurrentEXP() + 5);
        }

        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
        ASAIController* AIController = Cast<ASAIController>(GetController());
        if (true == ::IsValid(AIController))
        {
            AIController->EndAI();
        }
    }

    return FinalDamageAmount;
}

void ASNonPlayerCharacter::Attack()
{
    FHitResult HitResult;
    FCollisionQueryParams Params(NAME_None, false, this);   //this는 나 자신은 ignore한다는 뜻

    bool bResult = GetWorld()->SweepSingleByChannel(
        HitResult,
        GetActorLocation(),
        GetActorLocation() + GetActorForwardVector() * AttackRange,
        FQuat::Identity,
        ECC_GameTraceChannel2,
        FCollisionShape::MakeSphere(AttackRadius),
        Params
    );

    if (true == bResult)    //감지 성공
    {
        if (true == ::IsValid(HitResult.GetActor()))
        {
            UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("[NPC] Hit Actor Name : %s"), *HitResult.GetActor()->GetName()));
            
        }
    }

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (true == ::IsValid(AnimInstance))
    {
        AnimInstance->PlayAttackAnimMontage();
        bIsAttacking = true;
        if (false == AnimInstance->OnMontageEnded.IsAlreadyBound(this, &ThisClass::OnAttackAnimMontageEnded))
        {
            AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnAttackAnimMontageEnded);
        }
    }

#pragma region CollisionDebugDrawing
    FVector TraceVec = GetActorForwardVector() * AttackRange;
    FVector Center = GetActorLocation() + TraceVec * 0.5f;
    float HalfHeight = AttackRange * 0.5f + AttackRadius;
    FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();   //z축을 눕히는 과정 캡슐 모양이 눕혀진다. 
    FColor DrawColor = true == bResult ? FColor::Green : FColor::Red;
    float DebugLifeTime = 5.f;

    DrawDebugCapsule(
        GetWorld(),
        Center,
        HalfHeight,
        AttackRadius,
        CapsuleRot,
        DrawColor,
        false,
        DebugLifeTime
    );
#pragma endregion
}

void ASNonPlayerCharacter::OnAttackAnimMontageEnded(UAnimMontage* Montage, bool bIsInterrupt)
{
    bIsAttacking = false;
}
