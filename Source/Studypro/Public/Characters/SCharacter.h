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

	float GetMaxHP() const { return MaxHP; }

	float GetCurrentHP() const { return CurrentHP; }
	
	void SetMaxHP(float InMaxHP) { MaxHP = InMaxHP; }

	void SetCurrentHP(float InCurrentHP) { CurrentHP = InCurrentHP; }

	bool IsDead() const { return bIsDead; }
private:

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="ASCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASCharacter", Meta = (AllowprivateAccess))
	float MaxHP = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASCharacter", Meta = (AllowprivateAccess))
	float CurrentHP = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASCharacter", Meta = (AllowprivateAccess))
	uint8 bIsDead : 1;
};
