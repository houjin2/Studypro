// SPlayerPawn.cpp

#include "Characters/SPlayerPawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ASPlayerPawn::ASPlayerPawn()
{
    PrimaryActorTick.bCanEverTick = false;

    float CharacterHalfHeight = 90.f;
    float CharacterRadius = 40.f;

#pragma region InitializeCapsuleComponent
    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    SetRootComponent(CapsuleComponent);
    CapsuleComponent->SetCapsuleHalfHeight(CharacterHalfHeight);
    CapsuleComponent->SetCapsuleRadius(CharacterRadius);
#pragma endregion

#pragma region InitializeSkeletalMesh
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    SkeletalMeshComponent->SetupAttachment(RootComponent);
    FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
    FRotator PivotRotation(0.f, -90.f, 0.f);
    SkeletalMeshComponent->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);
    //이와같이 에셋을 지정해도 되지만 누군가 에셋의 경로를 바꿀수도 있으니 언리얼에서 교체하는게 바람직함.
    //static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("오브젝트 패스"));
    //if (true == SkeletalMeshAsset.Succeeded())
    //{
    //    SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshAsset.Object);
    //}
#pragma endregion

#pragma region InitializeCamera
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 400.f;
    SpringArmComponent->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);   //CameraComponent 는 SpringArmComponent에 붙여 SpringArmComponent에 따라 움직일수 있게 함 
#pragma endregion

    FloatingPawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovementComponent"));

}

void ASPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &ThisClass::UpDown);
    PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &ThisClass::LeftRight);
}

void ASPlayerPawn::BeginPlay()
{
    Super::BeginPlay();

}

void ASPlayerPawn::UpDown(float InAxisValue)
{
    AddMovementInput(GetActorForwardVector(), InAxisValue);
}

void ASPlayerPawn::LeftRight(float InAxisValue)
{
    AddMovementInput(GetActorRightVector(), InAxisValue);
}
