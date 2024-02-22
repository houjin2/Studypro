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

    bUseControllerRotationPitch = false;    //controlRotation값에 의해 Pawn이 회전하는것을 false 하곘다는 뜻 -> 마우스가 움직인다고 Pawn이 움직이지 않음!
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    SpringArmComponent->bUsePawnControlRotation = true; //카메라봉의 회전, 액터는 회전하지 않음!
    SpringArmComponent->bDoCollisionTest = true;    //카메라 봉이 지형을 통과할것이냐 true = 뚫지 않음
    SpringArmComponent->bInheritPitch = true;   //bInherit = RootComponent가 Control Rotation값을 가지고 오는데 그 값을 SpringArm이 갖고 있을 것이냐 -> 연동할것이냐
    SpringArmComponent->bInheritYaw = true;
    SpringArmComponent->bInheritRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;   //캐릭터 이동방향에 회전을 일치시키기
    GetCharacterMovement()->bUseControllerDesiredRotation = false;   //컨트롤로테이션을 목표 회전으로 삼고 지정한 속도로 돌리기. 위에 것을 false로 하고 이걸 true로 한다면 마우스 방향에 따라 고개가 바로 돌아감
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
