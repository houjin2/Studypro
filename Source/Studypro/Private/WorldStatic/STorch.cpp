// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/STorch.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
ASTorch::ASTorch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);


	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
	PointLightComponent->SetupAttachment(GetRootComponent());
	PointLightComponent->SetRelativeLocation(FVector(0.f, 0.f, 500.f));
	PointLightComponent->SetIntensity(10000);

	ParticleSystemCompnent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemCompnent"));
	ParticleSystemCompnent->SetupAttachment(GetRootComponent());
	ParticleSystemCompnent->SetRelativeLocation(FVector(0.f, 0.f, 500.f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
	if (true == ParticleSystem.Succeeded())
	{
		ParticleSystemCompnent->SetTemplate(ParticleSystem.Object);
	}

	BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
	BodyStaticMeshComponent->SetupAttachment(GetRootComponent());
	BodyStaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -30.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyStaticMesh(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Architecture/Pillar_50x500.Pillar_50x500'"));	//플레이중 경로 정보가 바뀌지 않으므로 static선언
	if (true == BodyStaticMesh.Succeeded())
	{
		BodyStaticMeshComponent->SetStaticMesh(BodyStaticMesh.Object);
	}

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
}

void ASTorch::BeginPlay()
{
	Super::BeginPlay();

	RotationSpeed = 300.f;
	RotatingMovementComponent->RotationRate = FRotator(0.f, RotationSpeed, 0.f);
}

void ASTorch::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//AddActorWorldRotation(FRotator(0.f, RotationSpeed*DeltaSeconds, 0.f));
}
