// SAnimInstance.cpp


#include "Animations/SAnimInstance.h"
#include "Characters/SRPGCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

USAnimInstance::USAnimInstance()
{
}

void USAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    CurrentSpeed = 0.0f;
    
    Velocity = FVector::ZeroVector;

    bIsFalling = false;

    bIsCrouching = false;

    bIsDead = false;
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    ASRPGCharacter* OwnerCharacter = Cast<ASRPGCharacter>(TryGetPawnOwner());   //Try�� ����ϴ� ���� ( ������ �Է� ���� -> ���� ���� -> �ִϸ��̼� ���� ������ ����Ǵµ� ���� �������� ���� �������ٸ�(�װų�) �ִϸ��̼��� ����� ������ ���� ������
    if (true == ::IsValid(OwnerCharacter))
    {
        UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
        if (true == ::IsValid(CharacterMovementComponent))
        {
            Velocity = CharacterMovementComponent->GetLastUpdateVelocity();
            CurrentSpeed = Velocity.Size();
            bIsFalling = CharacterMovementComponent->IsFalling();
            bIsCrouching = CharacterMovementComponent->IsCrouching();
            bIsDead = OwnerCharacter->IsDead();
        }
    }


}

void USAnimInstance::PlayAttackAnimMontage()
{
    if (true == ::IsValid(AttackAnimMontage))
    {
        if (false == Montage_IsPlaying(AttackAnimMontage))
        {
            Montage_Play(AttackAnimMontage);
        }
    }
}

void USAnimInstance::AnimNotify_CheckHit()
{
    if (true == OnCheckHitDelegate.IsBound())
    {
        OnCheckHitDelegate.Broadcast();
    }
}

void USAnimInstance::AnimNotify_CheckCanNextCombo()
{
    if (true == OnCheckCanNextComboDelegate.IsBound())
    {
        OnCheckCanNextComboDelegate.Broadcast();
    }
}