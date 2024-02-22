// SViewCharacter.cpp


#include "Characters/SViewCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Inputs/SInputConfigData.h"

ASViewCharacter::ASViewCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASViewCharacter::BeginPlay()
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

// �Է� ���� -> ���� ���� (Tick, possessedBy) -> �ִϸ��̼� ����
void ASViewCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    switch (CurrentViewMode)
    {
    case EViewMode::None:
        break;
    case EViewMode::Backview:
        break;
    case EViewMode::QuarterView:
        if (KINDA_SMALL_NUMBER < DirectionToMove.SizeSquared()) //DirectionToMove ���� �����̶� ���ϸ�.  ����ȭ-> ��ȭ�� ������ ���� ����
        {
            GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator()); //Matrix�� ����� �װſ� ���� Rotator�� ���� - �ϳ��� ���ͷκ��� ȸ�� ����� ���ϴ� �Լ�
            //MakeFromX(), MakeFromY(), MakeFromZ() ex) QuarterView ��Ŀ����� �� ���� �Է��� �ջ��� ���Ϳ�ĳ������ �ü� ����(X��)�� ���ƾ� �ϹǷ� MakeFromX()�� ����.
            AddMovementInput(DirectionToMove);
            DirectionToMove = FVector::ZeroVector;  //�̹� ƽ���� ��������ϱ� �ٽ� �ʱ�ȭ
        }
        break;
    case EViewMode::End:
        break;
    default:
        break;
    }

    if (KINDA_SMALL_NUMBER < abs(DestArmLength - SpringArmComponent->TargetArmLength))
    {
        SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, DestArmLength, DeltaSeconds, ArmLengthChangeSpeed);
        SpringArmComponent->SetRelativeRotation(FMath::RInterpTo(SpringArmComponent->GetRelativeRotation(), DestArmRotation, DeltaSeconds, ArmRotationChangeSpeed));
    }
}

void ASViewCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    SetViewMode(EViewMode::Backview);
    DestArmLength = 400.f;  //�ʱ�ȭ���� �� �� ��������� ��.
    DestArmRotation = FRotator::ZeroRotator;

}

void ASViewCharacter::SetViewMode(EViewMode InViewMode)
{
    if (CurrentViewMode == InViewMode)
    {
        return;
    }

    CurrentViewMode = InViewMode;

    switch (CurrentViewMode)
    {
    case EViewMode::None:
        break;
    case EViewMode::Backview:
        //SpringArmComponent->TargetArmLength = 400.f;
        //SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator); //ControlRotation�� Pawn�� ȸ���� ����ȭ -> Pawn�� ȸ���� SpringArm�� ȸ���� ����ȭ. �̷����� SetRotation()�� ���ǹ�

        bUseControllerRotationPitch = false;    //controlRotation���� ���� Pawn�� ȸ���ϴ°��� false �ρٴٴ� �� -> ���콺�� �����δٰ� Pawn�� �������� ����!
        bUseControllerRotationRoll = false;
        bUseControllerRotationYaw = false;

        SpringArmComponent->bUsePawnControlRotation = true; //ī�޶���� ȸ��, ���ʹ� ȸ������ ����!
        SpringArmComponent->bDoCollisionTest = true;    //ī�޶� ���� ������ ����Ұ��̳� true = ���� ����
        SpringArmComponent->bInheritPitch = true;   //bInherit = RootComponent�� Control Rotation���� ������ ���µ� �� ���� SpringArm�� ���� ���� ���̳� -> �����Ұ��̳�
        SpringArmComponent->bInheritRoll = false;
        SpringArmComponent->bInheritYaw = true;

        GetCharacterMovement()->bOrientRotationToMovement = true;   //ĳ���� �̵����⿡ ȸ���� ��ġ��Ű��
        GetCharacterMovement()->bUseControllerDesiredRotation = false;   //��Ʈ�ѷ����̼��� ��ǥ ȸ������ ��� ������ �ӵ��� ������. ���� ���� false�� �ϰ� �̰� true�� �Ѵٸ� ���콺 ���⿡ ���� ���� �ٷ� ���ư�
        GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

        break;
    case EViewMode::QuarterView:
        //SpringArmComponent->TargetArmLength = 900.f;
        //SpringArmComponent->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

        bUseControllerRotationPitch = false; 
        bUseControllerRotationRoll = false;
        bUseControllerRotationYaw = false;
        
        
        SpringArmComponent->bUsePawnControlRotation = false; //ī�޶���� ȸ��, ���ʹ� ȸ������ ����!
        SpringArmComponent->bDoCollisionTest = false;    //ī�޶� ���� ������ ����Ұ��̳� true = ���� ����
        SpringArmComponent->bInheritPitch = false;   //bInherit = RootComponent�� Control Rotation���� ������ ���µ� �� ���� SpringArm�� ���� ���� ���̳� -> �����Ұ��̳�
        SpringArmComponent->bInheritRoll = false;
        SpringArmComponent->bInheritYaw = false;

        GetCharacterMovement()->bOrientRotationToMovement = false;
        GetCharacterMovement()->bUseControllerDesiredRotation = true;
        GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
        break;
    case EViewMode::End:
        break;
    default:
        break;
    }
}

void ASViewCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (true == ::IsValid(EnhancedInputComponent))
    {
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MoveAction, ETriggerEvent::Triggered, this, &ASViewCharacter::Move);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LookAction, ETriggerEvent::Triggered, this, &ASViewCharacter::Look);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->ChangeViewAction, ETriggerEvent::Started, this, &ASViewCharacter::ChangeView);
    }
}

void ASViewCharacter::Move(const FInputActionValue& InValue)
{
    FVector2D MovementVector = InValue.Get<FVector2D>();

    switch (CurrentViewMode)
    {
    case EViewMode::None:
        break;
    case EViewMode::Backview:
    {
        const FRotator ControlRotation = GetController()->GetControlRotation();
        const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);  //�÷��̾��� ȸ�� ���� �� Yaw �������θ� ���� ������ ����

        const FVector ForWardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);    //ForwardVector = X��
        const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);      //RightVector = Y��

        AddMovementInput(ForWardVector, MovementVector.X);
        AddMovementInput(RightVector, MovementVector.Y);
        break;
    }
    case EViewMode::QuarterView:
        DirectionToMove.X = MovementVector.X;
        DirectionToMove.Y = MovementVector.Y;
        break;
    case EViewMode::End:
        break;
    default :
        AddMovementInput(GetActorForwardVector(), MovementVector.X);
        AddMovementInput(GetActorRightVector(), MovementVector.Y);
    }
    

}

void ASViewCharacter::Look(const FInputActionValue& InValue)
{
    FVector2D LookVector = InValue.Get<FVector2D>();

    switch (CurrentViewMode)
    {
    case EViewMode::None:
        break;
    case EViewMode::Backview:
        AddControllerYawInput(LookVector.X);
        AddControllerPitchInput(LookVector.Y);
        break;
    case EViewMode::QuarterView:
        break;
    case EViewMode::End:
        break;
    default:
        break;
    }
}

void ASViewCharacter::ChangeView(const FInputActionValue& InValue)
{
    switch (CurrentViewMode)
    {
    case EViewMode::None:
        break;
    case EViewMode::Backview:
        /* Case 1. ������ BackView �����̾��ٸ�

         BackView�� ��Ʈ�� ȸ���� == �������� ȸ����.
         �׷��� QuarterView�� ĳ������ ȸ���� == ��Ʈ�� ȸ����.
         ���� ���� ���� ���� ĳ������ ���� ȸ������ ��Ʈ�� ȸ���� �����ص־� ��.
         �ȱ׷��� ��Ʈ�� ȸ���� �Ͼ�鼭 ���� ĳ������ ȸ������ �������� ȸ����(��Ʈ�� ȸ����)���� ����ȭ��.
       */
        GetController()->SetControlRotation(GetActorRotation());
        DestArmLength = 900.f;
        DestArmRotation = FRotator(-45.f, 0.f, 0.f);
        SetViewMode(EViewMode::QuarterView);
        break;
    case EViewMode::QuarterView:
        /* Case 2. ������ QuarterView �����̾��ٸ�

          ��Ʈ�� ȸ���� ĳ���� ȸ���� ������ ��������.
          //QuarterView�� ���� ���������� ȸ������ ��Ʈ�� ȸ���� �����ص� ���¿��� ���� �����ؾ� �ùٸ�.
          BackView������ ��Ʈ�� �����̼��� ���� ȸ���� ����ȭ�ǰ� ���� ȸ���� ������ ���� ȸ���� ����ȭ.
          ���� ������ ���� ȸ���� ���Ƿ� ������ �� ����. 0���� ����.
        */
        GetController()->SetControlRotation(FRotator::ZeroRotator);
        DestArmLength = 400.f;
        DestArmRotation = FRotator::ZeroRotator;
        SetViewMode(EViewMode::Backview);
        break;
    case EViewMode::End:
        break;
    default:
        break;
    }
}
