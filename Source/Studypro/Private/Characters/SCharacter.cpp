// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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

	GetCharacterMovement()->MaxWalkSpeed = 500.f;	//�ȴ� �ӵ�
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;	//���̽�ƽ �����ӵ�
	GetCharacterMovement()->JumpZVelocity = 700.f;	//�������� �ӵ�
	GetCharacterMovement()->AirControl = 0.35f;	//���߿��� ��Ʈ��
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;	//���ߴ� �ӵ�

	bIsDead = false;

}


