// SCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class STUDYPRO_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASCharacter();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="ASTPSCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class UCameraComponent> CameraComponent;
};
