// SRPGCharacter.cpp


#include "Characters/SRPGCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Inputs/SInputConfigData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animations/SAnimInstance.h"
#include "Engine/DamageEvents.h"
#include "Engine/EngineTypes.h"

ASRPGCharacter::ASRPGCharacter()
    :bIsAttacking(false)    //Attacking �ʱ�ȭ
{
	PrimaryActorTick.bCanEverTick = false;

	CameraComponent->SetRelativeLocation(FVector(0.f, 100.f, 0.f));

    bUseControllerRotationPitch = false;    //controlRotation���� ���� Pawn�� ȸ���ϴ°��� false �ρٴٴ� �� -> ���콺�� �����δٰ� Pawn�� �������� ����!
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    SpringArmComponent->bUsePawnControlRotation = true; //ī�޶���� ȸ��, ���ʹ� ȸ������ ����!
    SpringArmComponent->bDoCollisionTest = true;    //ī�޶� ���� ������ ����Ұ��̳� true = ���� ����
    SpringArmComponent->bInheritPitch = true;   //bInherit = RootComponent�� Control Rotation���� ������ ���µ� �� ���� SpringArm�� ���� ���� ���̳� -> �����Ұ��̳�
    SpringArmComponent->bInheritYaw = true;
    SpringArmComponent->bInheritRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;   //ĳ���� �̵����⿡ ȸ���� ��ġ��Ű��
    GetCharacterMovement()->bUseControllerDesiredRotation = false;   //��Ʈ�ѷ����̼��� ��ǥ ȸ������ ��� ������ �ӵ��� ������. ���� ���� false�� �ϰ� �̰� true�� �Ѵٸ� ���콺 ���⿡ ���� ���� �ٷ� ���ư�
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

    GetCapsuleComponent()->SetCollisionProfileName(TEXT("SCharacter")); //�ش� ĳ������ �ݸ��� Ÿ���� SCharacter�� ����

}

void ASRPGCharacter::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (true == ::IsValid(PlayerController))
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
        if (true == ::IsValid(Subsystem))
        {
            Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
        }
    }

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (true == ::IsValid(AnimInstance))
    {
        AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnAttackMontageEnded);
        AnimInstance->OnCheckHitDelegate.AddDynamic(this, &ThisClass::CheckHit);
        AnimInstance->OnCheckCanNextComboDelegate.AddDynamic(this, &ThisClass::CheckCanNextCombo);
    }
}

void ASRPGCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    bIsAttacking = false;
}

float ASRPGCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float FinalDamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

    CurrentHP = FMath::Clamp(CurrentHP - FinalDamageAmount, 0.f, MaxHP);  //Clamp(X,Min,Max)  = (X < Min) ? Min : (X < Max) ? X : Max;

    if (CurrentHP < KINDA_SMALL_NUMBER)
    {
        bIsDead = true;
        CurrentHP = 0.f;
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    }
    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s [%.1f, %.1f]"), *GetName(), CurrentHP, MaxHP));

    return FinalDamageAmount;
}

void ASRPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (true == ::IsValid(EnhancedInputComponent))
    {
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MoveAction, ETriggerEvent::Triggered, this, &ASRPGCharacter::Move);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LookAction, ETriggerEvent::Triggered, this, &ASRPGCharacter::Look);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->JumpAction, ETriggerEvent::Started, this, &ASRPGCharacter::Jump);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Started, this, &ASRPGCharacter::Attack);
    }
}

void ASRPGCharacter::Move(const FInputActionValue& InValue)
{
    FVector2D MovementVector = InValue.Get<FVector2D>();
    ForwardInputValue = MovementVector.X;
    RightInputValue = MovementVector.Y;

    const FRotator CurrentControlRotation = GetController()->GetControlRotation();
    const FRotator CurrentControlRotationYaw(0.f, CurrentControlRotation.Yaw, 0.f);

    FVector ForwardDirection = FRotationMatrix(CurrentControlRotationYaw).GetUnitAxis(EAxis::X);
    FVector RightDirection = FRotationMatrix(CurrentControlRotationYaw).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.X);
    AddMovementInput(RightDirection, MovementVector.Y);
}

void ASRPGCharacter::Look(const FInputActionValue& InValue)
{
    FVector2D LookAxisVector = InValue.Get<FVector2D>();

    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void ASRPGCharacter::Attack(const FInputActionValue& InValue)
{
    //USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());    //�ϳ��� ���ݶ� ���
    //if (true == ::IsValid(AnimInstance) && bIsAttacking == false)
    //{
    //    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    //    AnimInstance->PlayAttackAnimMontage();
    //    bIsAttacking = true;
    //}

    if (0 == CurrentComboCount)
    {
        BeginCombo();
        return;
    }
    else
    {
        ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
        bIsAttackKeyPressed = true;
    }
}

void ASRPGCharacter::CheckHit()
{
    FHitResult HitResult;
    FCollisionQueryParams Params(NAME_None, false, this);   //this�� �� �ڽ��� ignore�Ѵٴ� ��

    bool bResult = GetWorld()->SweepSingleByChannel(
        HitResult,
        GetActorLocation(),
        GetActorLocation() + AttackRange,
        FQuat::Identity,
        ECC_GameTraceChannel2,
        FCollisionShape::MakeSphere(AttackRadius),
        Params
    );
    
    if (true == bResult)    //���� ����
    {
        if (true == ::IsValid(HitResult.GetActor()))
        {
            //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName()));
            FDamageEvent DamageEvent;   //#include "Engine/DamageEvents.h",  #include "Engine/EngineTypes.h"
            HitResult.GetActor()->TakeDamage(50.f, DamageEvent, GetController(), this);
        }
    }

#pragma region CollisionDebugDrawing
    FVector TraceVec = GetActorForwardVector() * AttackRange;
    FVector Center = GetActorLocation() + TraceVec * 0.5f;
    float HalfHeight = AttackRange * 0.5f + AttackRadius;
    FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();   //z���� ������ ���� ĸ�� ����� ��������. 
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

void ASRPGCharacter::BeginCombo()
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    CurrentComboCount = 1;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    AnimInstance->PlayAttackAnimMontage();

    
    FOnMontageEnded OnMontageEndedDelegate;
    OnMontageEndedDelegate.BindUObject(this, &ThisClass::EndCombo);
    AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->AttackAnimMontage);
}

void ASRPGCharacter::CheckCanNextCombo()
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    if (true == bIsAttackKeyPressed)
    {
        CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

        FName NextSectionName = *FString::Printf(TEXT("%s%d"), *AttackAnimMontageSectionName, CurrentComboCount);
        AnimInstance->Montage_JumpToSection(NextSectionName, AnimInstance->AttackAnimMontage);
        bIsAttackKeyPressed = false;
    }
}

void ASRPGCharacter::EndCombo(UAnimMontage* InAnimMontage, bool bInterrupted)
{
    ensure(0 != CurrentComboCount);
    CurrentComboCount = 0;
    bIsAttackKeyPressed = false;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

