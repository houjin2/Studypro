// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SStatComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	float CharacterHalfHeight = 90.f;
	float CharacterRadius = 40.f;

	GetCapsuleComponent()->InitCapsuleSize(CharacterRadius,CharacterHalfHeight);

	FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	GetCharacterMovement()->MaxWalkSpeed = 500.f;	//걷는 속도
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;	//조이스틱 죄저속도
	GetCharacterMovement()->JumpZVelocity = 700.f;	//떨어지는 속도
	GetCharacterMovement()->AirControl = 0.35f;	//공중에서 컨트롤
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;	//멈추는 속도

	StatComponent = CreateDefaultSubobject<USStatComponent>(TEXT("StatComponent"));

}

void ASCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (false == ::IsValid(StatComponent))
    {
        return;
    }

    if (false == StatComponent->OnOutOfCurrentHPDelegate.IsAlreadyBound(this, &ThisClass::OnCharacterDeath))
    {
        StatComponent->OnOutOfCurrentHPDelegate.AddDynamic(this, &ThisClass::OnCharacterDeath);
    }
}

float ASCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float FinalDamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

    StatComponent->SetCurrentHP(StatComponent->GetCurrentHP() - FinalDamageAmount);

    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s [%.1f / %.1f]"), *GetName(), StatComponent->GetCurrentHP(), StatComponent->GetMaxHP()));

    return FinalDamageAmount;
}

void ASCharacter::OnCharacterDeath()
{
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    if (true == StatComponent->OnOutOfCurrentHPDelegate.IsAlreadyBound(this, &ThisClass::OnCharacterDeath))
    {
        StatComponent->OnOutOfCurrentHPDelegate.RemoveDynamic(this, &ThisClass::OnCharacterDeath);
    }
}


