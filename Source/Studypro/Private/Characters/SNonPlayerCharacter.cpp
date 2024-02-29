// SNonPlayerCharacter.cpp


#include "Characters/SNonPlayerCharacter.h"
#include "Controllers/SAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

ASNonPlayerCharacter::ASNonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

    AIControllerClass = ASAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASNonPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;   //캐릭터 이동방향에 회전을 일치시키기
    GetCharacterMovement()->bUseControllerDesiredRotation = true;   //컨트롤로테이션을 목표 회전으로 삼고 지정한 속도로 돌리기. 위에 것을 false로 하고 이걸 true로 한다면 마우스 방향에 따라 고개가 바로 돌아감
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

    GetCharacterMovement()->MaxWalkSpeed = 300.f;
}
