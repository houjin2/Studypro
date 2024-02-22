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

ASRPGCharacter::ASRPGCharacter()
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
}

void ASRPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (true == ::IsValid(EnhancedInputComponent))
    {
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MoveAction, ETriggerEvent::Triggered, this, &ASRPGCharacter::Move);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LookAction, ETriggerEvent::Triggered, this, &ASRPGCharacter::Look);
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
