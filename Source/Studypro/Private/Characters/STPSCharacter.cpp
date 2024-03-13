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

    CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, 35.f);
    CameraComponent->SetFieldOfView(CurrentFOV);
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
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->IronSightAction, ETriggerEvent::Started, this, &ThisClass::StartIronSight);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->IronSightAction, ETriggerEvent::Completed, this, &ThisClass::EndIronSight);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->TriggerAction, ETriggerEvent::Started, this, &ThisClass::ToggleTrigger);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Started, this, &ThisClass::StartFire);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Completed, this, &ThisClass::StopFire);
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
    bool bIsCollide = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, CameraEndLocation, ECC_Visibility, QueryParams);
    //DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0, 255), false, 20.f, 0U, 5.f);
    //DrawDebugSphere(GetWorld(), Start, 3.f, 16, FColor(0, 255, 0, 255), false, 20.f, 0U, 5.f);
    //DrawDebugSphere(GetWorld(), End, 3.f, 16, FColor(0, 0, 255, 255), false, 20.f, 0U, 5.f);

    if (true == bIsCollide)
    {
        DrawDebugLine(GetWorld(), MuzzleLocation, HitResult.Location, FColor(255, 255, 255, 64), true, 0.1f, 0u, 0.5f);
    }
    else
    {
        DrawDebugLine(GetWorld(), MuzzleLocation, CameraEndLocation, FColor(255, 255, 255, 64), false, 0.1f, 0U, 0.5f);
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
