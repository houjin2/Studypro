// SRPGCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Characters/SCharacter.h"
#include "InputActionValue.h"
#include "SRPGCharacter.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API ASRPGCharacter : public ASCharacter
{
	GENERATED_BODY()
	
public:
	ASRPGCharacter();

	virtual void BeginPlay() override;

	float GetForwardInputValue() const { return ForwardInputValue; }

	float GetRightInputValue() const { return RightInputValue; }

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Move(const FInputActionValue& InValue);

	void Look(const FInputActionValue& InValue);

private:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SRPGCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SRPGCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SRPGCharacter", Meta = (AllowprivateAccess = true))
	float ForwardInputValue;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SRPGCharacter", Meta = (AllowprivateAccess = true))
	float RightInputValue;
};
