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
    GetCharacterMovement()->bUseControllerDesiredRotation = true;   //��Ʈ�ѷ����̼��� ��ǥ ȸ������ ��� ������ �ӵ��� ������. ���� ���� false�� �ϰ� �̰� true�� �Ѵٸ� ���콺 ���⿡ ���� ���� �ٷ� ���ư�
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
    // �浹 ���� �ÿ� �� �� ������ ����� �浹ü�� �������� ���������� ���� �Ӽ�.
    // Content Browser > StarterContent > Props > SM_Chair ����Ŭ��.
    // Details > Collision Complexity�� Use Complex Collision As Simple�� �����ϸ�
    // ����(�浹 ����) ��û�� ������ ��翡 ���� ������ ������.
    // �浹 ��� ���ϴ� ���������� �׸�ŭ �������� ���� �÷��� ����.
    // SM_Chair > Toolbar > Collision > Auto Convex Collision Ŭ�� �� ���ϴ� Convex Decomposition����
    // ������ ����� �浹ü�� �ս��� ���� ����.

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
