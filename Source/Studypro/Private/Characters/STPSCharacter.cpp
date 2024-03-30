// STPSCharacter.cpp


#include "Characters/STPSCharacter.h"
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
#include "Kismet/KismetMathLibrary.h"   //��ݱ���
#include "Controllers/SPlayerController.h"  //CameraShake
#include "Components/SStatComponent.h"  //�ǰ�
#include "Engine/EngineTypes.h"         //�ǰ�
#include "Engine/DamageEvents.h"        //�ǰ�
#include "WorldStatic/SLandMine.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"      //�ִϸ��̼� ����ȭ
#include "Engine/Engine.h"          //�ִϸ��̼� ����ȭ

ASTPSCharacter::ASTPSCharacter()
    : ASCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCollisionProfileName(TEXT("SCharacter")); //�ش� ĳ������ �ݸ��� Ÿ���� SCharacter�� ����

    CameraComponent->SetRelativeLocation(FVector(0.f, 60.f, 20.f)); //TPS����� ���ð��� Ư¡

    bUseControllerRotationPitch = false;    //controlRotation���� ���� Pawn�� ȸ���ϴ°��� false �ρٴٴ� �� -> ���콺�� �����δٰ� Pawn�� �������� ����!
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    SpringArmComponent->bUsePawnControlRotation = true; //ī�޶���� ȸ��, ���ʹ� ȸ������ ����!
    SpringArmComponent->bDoCollisionTest = true;    //ī�޶� ���� ������ ����Ұ��̳� true = ���� ����
    SpringArmComponent->bInheritPitch = true;   //bInherit = RootComponent�� Control Rotation���� ������ ���µ� �� ���� SpringArm�� ���� ���� ���̳� -> �����Ұ��̳�
    SpringArmComponent->bInheritYaw = true;
    SpringArmComponent->bInheritRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;   //ĳ���� �̵����⿡ ȸ���� ��ġ��Ű��
    GetCharacterMovement()->bUseControllerDesiredRotation = true;   //��Ʈ�ѷ����̼��� ��ǥ ȸ������ ��� ������ �ӵ��� ������. ���� ���� false�� �ϰ� �̰� true�� �Ѵٸ� ���콺 ���⿡ ���� ������ �ٷ� ���ư�
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

    WeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMeshComponent"));

    TimeBetweenFire = 60.f / FirePerMinute;
}

void ASTPSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, 35.f);   //��Ŭ�� �� ����
    CameraComponent->SetFieldOfView(CurrentFOV);
    

    // Aim ���⿡ ���� �ѱ� ����
    if (true == ::IsValid(GetController()))
    {
        FRotator ControlRotation = GetController()->GetControlRotation();
        CurrentAimPitch = ControlRotation.Pitch;
        CurrentAimYaw = ControlRotation.Yaw;
    }

    //�κ� ���� �ǰ� ����
    if (true == bIsNowRagdollBlending)
    {
        CurrentRagdollBlendWeight = FMath::FInterpTo(CurrentRagdollBlendWeight, TargetRagdollBlendWeight, DeltaSeconds, 10.f);

        FName PivotBoneName = FName(TEXT("spine_01"));
        GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, CurrentRagdollBlendWeight);

        if (CurrentRagdollBlendWeight - TargetRagdollBlendWeight < KINDA_SMALL_NUMBER)
        {
            GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, false);
            bIsNowRagdollBlending = false;
        }

        //�ǰݸ�� ���� ������ �ٽ� ��ü�� ���� ������� ���ư��� ������ ü���� 0�� ������ ��ü ���� Ȱ��ȭ
        if (true == ::IsValid(GetStatComponent()) && GetStatComponent()->GetCurrentHP() < KINDA_SMALL_NUMBER)
        {
            GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName(TEXT("root")), 1.f);
            // ��� ���� ���� ����ġ
            GetMesh()->SetSimulatePhysics(true);
            bIsNowRagdollBlending = false;
        }
    }


    //�ִϸ��̼� ����ȭ
    if (true == ::IsValid(GetController()))
    {
        PreviousAimPitch = CurrentAimPitch;
        PreviousAimYaw = CurrentAimYaw;

        FRotator ControlRotation = GetController()->GetControlRotation();
        CurrentAimPitch = ControlRotation.Pitch;
        CurrentAimYaw = ControlRotation.Yaw;

        if (PreviousAimPitch != CurrentAimPitch || PreviousAimYaw != CurrentAimYaw)
        {
            if (false == HasAuthority() || GetOwner() != UGameplayStatics::GetPlayerController(this, 0)) //�������� ���� Update ���� �����Ƿ� ȣ���� �ʿ� ����.
            {
                UpdateAimValue_Server(CurrentAimPitch, CurrentAimYaw);
            }
        }
    }

    if (PreviousForwardInputValue != ForwardInputValue || PreviousRightInputValue != RightInputValue)
    {
        if (false == HasAuthority() || GetOwner() != UGameplayStatics::GetPlayerController(this, 0))    //�������� ���� Update ���� �����Ƿ� ȣ���� �ʿ� ����.
        {
            UpdateInputValue_Server(ForwardInputValue, RightInputValue);
        }
    }

}

void ASTPSCharacter::BeginPlay()
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

    FName EquipmentSocketName = FName(TEXT("EquipmentSocket"));
    if (true == GetMesh()->DoesSocketExist(EquipmentSocketName))
    {
        WeaponSkeletalMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, EquipmentSocketName);
    }
}

float ASTPSCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

    PlayRagdoll_NetMulticast();

    return ActualDamage;
}

void ASTPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, ForwardInputValue);
    DOREPLIFETIME(ThisClass, RightInputValue);
    DOREPLIFETIME(ThisClass, CurrentAimPitch);
    DOREPLIFETIME(ThisClass, CurrentAimYaw);
}

void ASTPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (true == ::IsValid(EnhancedInputComponent))
    {
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MoveAction, ETriggerEvent::Triggered, this, &ASTPSCharacter::Move);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LookAction, ETriggerEvent::Triggered, this, &ASTPSCharacter::Look);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->JumpAction, ETriggerEvent::Started, this, &ASTPSCharacter::Jump);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Started, this, &ASTPSCharacter::Attack);
        //��Ŭ�� ��
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->IronSightAction, ETriggerEvent::Started, this, &ThisClass::StartIronSight);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->IronSightAction, ETriggerEvent::Completed, this, &ThisClass::EndIronSight);
        //�ܹ� ����
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->TriggerAction, ETriggerEvent::Started, this, &ThisClass::ToggleTrigger);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Started, this, &ThisClass::StartFire);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Completed, this, &ThisClass::StopFire);
        //LandMine
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LandMineAction, ETriggerEvent::Started, this, &ThisClass::SpawnLandMine);

    }
}

void ASTPSCharacter::Move(const FInputActionValue& InValue)
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

void ASTPSCharacter::Look(const FInputActionValue& InValue)
{
    FVector2D LookAxisVector = InValue.Get<FVector2D>();

    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void ASTPSCharacter::Attack(const FInputActionValue& InValue)
{
    if (false == bIsTriggerToggle)
    {
        Fire();
    }
}

void ASTPSCharacter::Fire()
{
    //Fire()��ü�� Owning Client���� ����Ǿ�� �ϱ� ������
    if (true == HasAuthority() || GetOwner() != UGameplayStatics::GetPlayerController(this, 0))
    {
        return;
    }

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (false == ::IsValid(PlayerController))
    {
        return;
    }
    FHitResult HitResult;

    FVector CameraStartLocation = CameraComponent->GetComponentLocation();
    FVector CameraEndLocation = CameraStartLocation + CameraComponent->GetForwardVector() * 5000.f;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredComponent((const UPrimitiveComponent*)(CameraComponent));
    QueryParams.bTraceComplex = true;
    // �浹 ���� �ÿ� �� �� ������ ����� �浹ü�� �������� ���������� ���� �Ӽ�.
    // Content Browser > StarterContent > Props > SM_Chair ����Ŭ��.
    // Details > Collision Complexity�� Use Complex Collision As Simple�� �����ϸ�
    // ����(�浹 ����) ��û�� ������ ��翡 ���� ������ ������.
    // �浹 ��� ���ϴ� ���������� �׸�ŭ �������� ���� �÷��� ����.
    // SM_Chair > Toolbar > Collision > Auto Convex Collision Ŭ�� �� ���ϴ� Convex Decomposition����
    // ������ ����� �浹ü�� �ս��� ���� ����.

    FVector MuzzleLocation = WeaponSkeletalMeshComponent->GetSocketLocation(FName("MuzzleSocket"));
    bool bIsCollide = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, CameraEndLocation, ECC_GameTraceChannel2, QueryParams);   //ECC_Visibility�� ĸ�� ������Ʈ�� ���� ����. �Ȱ��� Attack�� Ʈ���̽� ä�η� ����.
    //DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0, 255), false, 20.f, 0U, 5.f);
    //DrawDebugSphere(GetWorld(), Start, 3.f, 16, FColor(0, 255, 0, 255), false, 20.f, 0U, 5.f);
    //DrawDebugSphere(GetWorld(), End, 3.f, 16, FColor(0, 0, 255, 255), false, 20.f, 0U, 5.f);

    if (true == bIsCollide)
    {
        //DrawDebugLine(GetWorld(), MuzzleLocation, HitResult.Location, FColor(255, 255, 255, 64), true, 0.1f, 0u, 0.5f);

        ASCharacter* HittedCharacter = Cast<ASCharacter>(HitResult.GetActor());
        if (true == ::IsValid(HittedCharacter))
        {
            FDamageEvent DamageEvent;
            //HittedCharacter->TakeDamage(10.f, DamageEvent, GetController(), this);

            //�ǰ� ���� ���� ����
            FString BoneNameString = HitResult.BoneName.ToString();
            //UKismetSystemLibrary::PrintString(this, BoneNameString);
            //DrawDebugSphere(GetWorld(), HitResult.Location, 3.f, 16, FColor(255, 0, 0, 255), true, 20.f, 0U, 5.f);

            //��弦 ���� �߰�
            if (true == BoneNameString.Equals(FString(TEXT("HEAD")), ESearchCase::IgnoreCase))  //ESearchCast�� HEAD���� ��ҹ��� ���о��ϰ� ���ڸ� ������ �ǰ� �Ҷ� ���
            {
                //HittedCharacter->TakeDamage(100.f, DamageEvent, GetController(), this);
                ApplyDamageAndDrawLine_Server(MuzzleLocation, HitResult.Location, HittedCharacter, 100.f, DamageEvent, GetController(), this);
            }
            else
            {
                //HittedCharacter->TakeDamage(10.f, DamageEvent, GetController(), this);
                ApplyDamageAndDrawLine_Server(MuzzleLocation, HitResult.Location, HittedCharacter, 10.f, DamageEvent, GetController(), this);
            }
        }
    }
    else
    {
        //DrawDebugLine(GetWorld(), MuzzleLocation, CameraEndLocation, FColor(255, 255, 255, 64), false, 0.1f, 0U, 0.5f);
        FDamageEvent DamageEvent;
        ApplyDamageAndDrawLine_Server(MuzzleLocation, CameraEndLocation, nullptr, 0.f, DamageEvent, GetController(), this);
    }


    //AnimMontage ����
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (false == AnimInstance)
    {
        return;
    }

    if (false == AnimInstance->Montage_IsPlaying(RifleFireAnimMontage))
    {
        AnimInstance->Montage_Play(RifleFireAnimMontage);
        PlayAttackMontage_Server();
    }


    //CameraShake
    if (true == ::IsValid(FireShake))
    {
        if(GetOwner() == UGameplayStatics::GetPlayerController(this, 0))
        { 
            PlayerController->ClientStartCameraShake(FireShake);
        }

    }

}

void ASTPSCharacter::StartIronSight(const FInputActionValue& InValue)
{
    TargetFOV = 45.f;
}

void ASTPSCharacter::EndIronSight(const FInputActionValue& InValue)
{
    TargetFOV = 70.f;
}

void ASTPSCharacter::ToggleTrigger(const FInputActionValue& InValue)
{
    bIsTriggerToggle = !bIsTriggerToggle;
}

void ASTPSCharacter::StartFire(const FInputActionValue& InValue)
{
    if (true == bIsTriggerToggle)
    {
        GetWorldTimerManager().SetTimer(BetweenShotsTimer, this, &ThisClass::Fire, TimeBetweenFire, true);
    }
}

void ASTPSCharacter::StopFire(const FInputActionValue& InValue)
{
    GetWorldTimerManager().ClearTimer(BetweenShotsTimer);
}

void ASTPSCharacter::SpawnLandMine(const FInputActionValue& InValue)
{
    SpawnLandMine_Server();
}

void ASTPSCharacter::OnHittedRagdollRestoreTimerElapsed()
{
    FName PivotBoneName = FName(TEXT("spine_01"));
    //GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, false);
    //float BlendWeight = 0.f;
    //GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, BlendWeight);
    TargetRagdollBlendWeight = 0.f;
    CurrentRagdollBlendWeight = 1.f;
    bIsNowRagdollBlending = true;
}

void ASTPSCharacter::PlayAttackMontage_Server_Implementation()
{
    PlayAttackMontage_NetMulticast();
}

void ASTPSCharacter::PlayAttackMontage_NetMulticast_Implementation()
{
    if (false == HasAuthority() && GetOwner() != UGameplayStatics::GetPlayerController(this, 0))
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (false == ::IsValid(AnimInstance))
        {
            return;
        }

        if (false == AnimInstance->Montage_IsPlaying(RifleFireAnimMontage))
        {
            AnimInstance->Montage_Play(RifleFireAnimMontage);
        }
    }
}

void ASTPSCharacter::UpdateInputValue_Server_Implementation(const float& InForwardInputValue, const float& InRightInputValue)
{
    ForwardInputValue = InForwardInputValue;
    RightInputValue = InRightInputValue;
}

void ASTPSCharacter::UpdateAimValue_Server_Implementation(const float& InAimPitchValue, const float& InAimYawValue)
{
    CurrentAimPitch = InAimPitchValue;
    CurrentAimYaw = InAimYawValue;
}

bool ASTPSCharacter::SpawnLandMine_Server_Validate()    //������ �Լ�
{
    return true;
}

void ASTPSCharacter::SpawnLandMine_Server_Implementation()  //Implementation() �� �����Լ�
{
    if (true == ::IsValid(LandMineClass))
    {
        FVector SpawnedLocation = (GetActorLocation() + GetActorForwardVector() * 200.f) - FVector(0.f, 0.f, 90.f);
        ASLandMine* SpawnedLandMine = GetWorld()->SpawnActor<ASLandMine>(LandMineClass, SpawnedLocation, FRotator::ZeroRotator);
        SpawnedLandMine->SetOwner(GetController());
    }
}

void ASTPSCharacter::ApplyDamageAndDrawLine_Server_Implementation(const FVector& InDrawStart, const FVector& InDrawEnd, ACharacter* InHittedCharacter, float InDamage, FDamageEvent const& InDamageEvent, AController* InEventInstigator, AActor* InDamageCauser)
{
    if (true == ::IsValid(InHittedCharacter))
    {
        InHittedCharacter->TakeDamage(InDamage, InDamageEvent, InEventInstigator, InDamageCauser);
    }

    DrawLine_NetMulticast(InDrawStart, InDrawEnd);
}

void ASTPSCharacter::DrawLine_NetMulticast_Implementation(const FVector& InDrawStart, const FVector& InDrawEnd)
{
    DrawDebugLine(GetWorld(), InDrawStart, InDrawEnd, FColor(255, 255, 255, 64), false, 0.1f, 0U, 0.5f);
}

void ASTPSCharacter::PlayRagdoll_NetMulticast_Implementation()
{
    if (false == ::IsValid(GetStatComponent()))
    {
        return;
    }

    if (GetStatComponent()->GetCurrentHP() < KINDA_SMALL_NUMBER)
    {
        GetMesh()->SetSimulatePhysics(true);    //���� Ȱ��ȭ -> ������ �幰�幰 
    }
    else   //�κ� ��������Ͽ� �ǰݸ��
    {
        FName PivotBoneName = FName(TEXT("spine_01"));
        GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, true);
        //float BlendWeight = 1.f;    //���� ��� ���� ġ�������Բ� ����ġ ����
        //GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, BlendWeight);

        TargetRagdollBlendWeight = 1.f;
        HittedRagdollRestoreTimerDelegate.BindUObject(this, &ThisClass::OnHittedRagdollRestoreTimerElapsed);
        GetWorld()->GetTimerManager().SetTimer(HittedRagdollRestoreTimer, HittedRagdollRestoreTimerDelegate, 1.f, false);
    }
}