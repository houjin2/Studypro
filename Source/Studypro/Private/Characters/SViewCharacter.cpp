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

// 입력 로직 -> 게임 로직 (Tick, possessedBy) -> 애니메이션 로직
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
        if (KINDA_SMALL_NUMBER < DirectionToMove.SizeSquared()) //DirectionToMove 값이 조금이라도 변하면.  최적화-> 변화가 없으면 돌지 않음
        {
            GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator()); //Matrix를 만들고 그거에 대한 Rotator를 만듬 - 하나의 벡터로부터 회전 행렬을 구하는 함수
            //MakeFromX(), MakeFromY(), MakeFromZ() ex) QuarterView 방식에서는 두 축의 입력을 합산한 벡터와캐릭터의 시선 방향(X축)이 같아야 하므로 MakeFromX()가 사용됨.
            AddMovementInput(DirectionToMove);
            DirectionToMove = FVector::ZeroVector;  //이번 틱에서 사용햇으니까 다시 초기화
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
    DestArmLength = 400.f;  //초기화에서 한 번 지정해줘야 함.
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
        //SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator); //ControlRotation이 Pawn의 회전과 동기화 -> Pawn의 회전이 SpringArm의 회전과 동기화. 이로인해 SetRotation()이 무의미

        bUseControllerRotationPitch = false;    //controlRotation값에 의해 Pawn이 회전하는것을 false 하곘다는 뜻 -> 마우스가 움직인다고 Pawn이 움직이지 않음!
        bUseControllerRotationRoll = false;
        bUseControllerRotationYaw = false;

        SpringArmComponent->bUsePawnControlRotation = true; //카메라봉의 회전, 액터는 회전하지 않음!
        SpringArmComponent->bDoCollisionTest = true;    //카메라 봉이 지형을 통과할것이냐 true = 뚫지 않음
        SpringArmComponent->bInheritPitch = true;   //bInherit = RootComponent가 Control Rotation값을 가지고 오는데 그 값을 SpringArm이 갖고 있을 것이냐 -> 연동할것이냐
        SpringArmComponent->bInheritRoll = false;
        SpringArmComponent->bInheritYaw = true;

        GetCharacterMovement()->bOrientRotationToMovement = true;   //캐릭터 이동방향에 회전을 일치시키기
        GetCharacterMovement()->bUseControllerDesiredRotation = false;   //컨트롤로테이션을 목표 회전으로 삼고 지정한 속도로 돌리기. 위에 것을 false로 하고 이걸 true로 한다면 마우스 방향에 따라 고개가 바로 돌아감
        GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

        break;
    case EViewMode::QuarterView:
        //SpringArmComponent->TargetArmLength = 900.f;
        //SpringArmComponent->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

        bUseControllerRotationPitch = false; 
        bUseControllerRotationRoll = false;
        bUseControllerRotationYaw = false;
        
        
        SpringArmComponent->bUsePawnControlRotation = false; //카메라봉의 회전, 액터는 회전하지 않음!
        SpringArmComponent->bDoCollisionTest = false;    //카메라 봉이 지형을 통과할것이냐 true = 뚫지 않음
        SpringArmComponent->bInheritPitch = false;   //bInherit = RootComponent가 Control Rotation값을 가지고 오는데 그 값을 SpringArm이 갖고 있을 것이냐 -> 연동할것이냐
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
        const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);  //플레이어의 회전 의지 중 Yaw 성분으로만 전진 방향을 결정

        const FVector ForWardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);    //ForwardVector = X축
        const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);      //RightVector = Y출

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
        /* Case 1. 이전에 BackView 시점이었다면

         BackView는 컨트롤 회전값 == 스프링암 회전값.
         그러나 QuarterView는 캐릭터의 회전값 == 컨트롤 회전값.
         따라서 시점 변경 전에 캐릭터의 현재 회전값을 컨트롤 회전에 세팅해둬야 함.
         안그러면 컨트롤 회전이 일어나면서 현재 캐릭터의 회전값이 스프링암 회전값(컨트롤 회전값)으로 동기화됨.
       */
        GetController()->SetControlRotation(GetActorRotation());
        DestArmLength = 900.f;
        DestArmRotation = FRotator(-45.f, 0.f, 0.f);
        SetViewMode(EViewMode::QuarterView);
        break;
    case EViewMode::QuarterView:
        /* Case 2. 이전에 QuarterView 시점이었다면

          컨트롤 회전이 캐릭터 회전에 맞춰져 있을거임.
          //QuarterView는 현재 스프링암의 회전값을 컨트롤 회전에 세팅해둔 상태에서 시점 변경해야 올바름.
          BackView에서는 컨트롤 로테이션이 폰의 회전과 동기화되고 폰의 회전이 스프링 암의 회전과 동기화.
          따라서 스프링 암의 회전을 임의로 설정할 수 없음. 0으로 고정.
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
