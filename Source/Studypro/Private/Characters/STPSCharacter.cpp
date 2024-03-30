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
#include "Kismet/KismetMathLibrary.h"   //사격구현
#include "Controllers/SPlayerController.h"  //CameraShake
#include "Components/SStatComponent.h"  //피격
#include "Engine/EngineTypes.h"         //피격
#include "Engine/DamageEvents.h"        //피격
#include "WorldStatic/SLandMine.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"      //애니메이션 동기화
#include "Engine/Engine.h"          //애니메이션 동기화

ASTPSCharacter::ASTPSCharacter()
    : ASCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCollisionProfileName(TEXT("SCharacter")); //해당 캐릭터의 콜리젼 타입을 SCharacter로 설정

    CameraComponent->SetRelativeLocation(FVector(0.f, 60.f, 20.f)); //TPS방식의 슈팅게임 특징

    bUseControllerRotationPitch = false;    //controlRotation값에 의해 Pawn이 회전하는것을 false 하곘다는 뜻 -> 마우스가 움직인다고 Pawn이 움직이지 않음!
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    SpringArmComponent->bUsePawnControlRotation = true; //카메라봉의 회전, 액터는 회전하지 않음!
    SpringArmComponent->bDoCollisionTest = true;    //카메라 봉이 지형을 통과할것이냐 true = 뚫지 않음
    SpringArmComponent->bInheritPitch = true;   //bInherit = RootComponent가 Control Rotation값을 가지고 오는데 그 값을 SpringArm이 갖고 있을 것이냐 -> 연동할것이냐
    SpringArmComponent->bInheritYaw = true;
    SpringArmComponent->bInheritRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;   //캐릭터 이동방향에 회전을 일치시키기
    GetCharacterMovement()->bUseControllerDesiredRotation = true;   //컨트롤로테이션을 목표 회전으로 삼고 지정한 속도로 돌리기. 위에 것을 false로 하고 이걸 true로 한다면 마우스 방향에 따라 고개가 바로 돌아감
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

    WeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMeshComponent"));

    TimeBetweenFire = 60.f / FirePerMinute;
}

void ASTPSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, 35.f);   //우클릭 줌 구현
    CameraComponent->SetFieldOfView(CurrentFOV);
    

    // Aim 방향에 따른 총구 방향
    if (true == ::IsValid(GetController()))
    {
        FRotator ControlRotation = GetController()->GetControlRotation();
        CurrentAimPitch = ControlRotation.Pitch;
        CurrentAimYaw = ControlRotation.Yaw;
    }

    //부분 렉돌 피격 구현
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

        //피격모션 도중 죽으면 다시 상체가 기존 모션으로 돌아가기 때문에 체력이 0과 가까우면 전체 렉돌 활성화
        if (true == ::IsValid(GetStatComponent()) && GetStatComponent()->GetCurrentHP() < KINDA_SMALL_NUMBER)
        {
            GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName(TEXT("root")), 1.f);
            // 모든 본에 렉돌 가중치
            GetMesh()->SetSimulatePhysics(true);
            bIsNowRagdollBlending = false;
        }
    }


    //애니메이션 동기화
    if (true == ::IsValid(GetController()))
    {
        PreviousAimPitch = CurrentAimPitch;
        PreviousAimYaw = CurrentAimYaw;

        FRotator ControlRotation = GetController()->GetControlRotation();
        CurrentAimPitch = ControlRotation.Pitch;
        CurrentAimYaw = ControlRotation.Yaw;

        if (PreviousAimPitch != CurrentAimPitch || PreviousAimYaw != CurrentAimYaw)
        {
            if (false == HasAuthority() || GetOwner() != UGameplayStatics::GetPlayerController(this, 0)) //서버에선 원래 Update 되지 않으므로 호출할 필요 없음.
            {
                UpdateAimValue_Server(CurrentAimPitch, CurrentAimYaw);
            }
        }
    }

    if (PreviousForwardInputValue != ForwardInputValue || PreviousRightInputValue != RightInputValue)
    {
        if (false == HasAuthority() || GetOwner() != UGameplayStatics::GetPlayerController(this, 0))    //서버에선 원래 Update 되지 않으므로 호출할 필요 없음.
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
        //우클릭 줌
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->IronSightAction, ETriggerEvent::Started, this, &ThisClass::StartIronSight);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->IronSightAction, ETriggerEvent::Completed, this, &ThisClass::EndIronSight);
        //단발 연발
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
    //Fire()자체는 Owning Client에서 수행되어야 하기 떄문에
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
    // 충돌 감지 시에 좀 더 복잡한 모양의 충돌체를 기준으로 감지할지에 대한 속성.
    // Content Browser > StarterContent > Props > SM_Chair 더블클릭.
    // Details > Collision Complexity를 Use Complex Collision As Simple로 설정하면
    // 쿼리(충돌 검지) 요청시 복잡한 모양에 대한 쿼리를 제공함.
    // 충돌 계산 부하는 증가하지만 그만큼 현실적인 게임 플레이 가능.
    // SM_Chair > Toolbar > Collision > Auto Convex Collision 클릭 후 우하단 Convex Decomposition으로
    // 복잡한 모양의 충돌체를 손쉽게 제작 가능.

    FVector MuzzleLocation = WeaponSkeletalMeshComponent->GetSocketLocation(FName("MuzzleSocket"));
    bool bIsCollide = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, CameraEndLocation, ECC_GameTraceChannel2, QueryParams);   //ECC_Visibility는 캡슐 컴포넌트를 뚫지 못함. 똑같이 Attack의 트레이스 채널로 변경.
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

            //피격 부위 판정 구현
            FString BoneNameString = HitResult.BoneName.ToString();
            //UKismetSystemLibrary::PrintString(this, BoneNameString);
            //DrawDebugSphere(GetWorld(), HitResult.Location, 3.f, 16, FColor(255, 0, 0, 255), true, 20.f, 0U, 5.f);

            //헤드샷 로직 추가
            if (true == BoneNameString.Equals(FString(TEXT("HEAD")), ESearchCase::IgnoreCase))  //ESearchCast는 HEAD에서 대소문자 구분안하고 글자만 맞으면 되게 할때 사용
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


    //AnimMontage 연결
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

bool ASTPSCharacter::SpawnLandMine_Server_Validate()    //검증용 함수
{
    return true;
}

void ASTPSCharacter::SpawnLandMine_Server_Implementation()  //Implementation() 이 구현함수
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
        GetMesh()->SetSimulatePhysics(true);    //렉돌 활성화 -> 죽을시 흐물흐물 
    }
    else   //부분 렉돌사용하여 피격모션
    {
        FName PivotBoneName = FName(TEXT("spine_01"));
        GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, true);
        //float BlendWeight = 1.f;    //렉돌 포즈에 완전 치우쳐지게끔 가중치 설정
        //GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, BlendWeight);

        TargetRagdollBlendWeight = 1.f;
        HittedRagdollRestoreTimerDelegate.BindUObject(this, &ThisClass::OnHittedRagdollRestoreTimerElapsed);
        GetWorld()->GetTimerManager().SetTimer(HittedRagdollRestoreTimer, HittedRagdollRestoreTimerDelegate, 1.f, false);
    }
}
