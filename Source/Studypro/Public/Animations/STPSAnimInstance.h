// STPSAnimInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "STPSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API USTPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	USTPSAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "STPSAnimInstance", Meta = (AllowprivateAccess = true))
	TObjectPtr<class ASTPSCharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "STPSAnimInstance", Meta = (AllowprivateAccess = true))
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "STPSAnimInstance", Meta = (AllowprivateAccess = true))
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "STPSAnimInstance", Meta = (AllowprivateAccess = true))
	FVector MoveInputWithMaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "STPSAnimInstance", Meta = (AllowPrivateAccess = true))
	FVector MoveInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "STPSAnimInstance", Meta = (AllowPrivateAccess = true))
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "STPSAnimInstance", Meta = (AllowPrivateAccess = true))
	uint8 bIsDead : 1;
};
