// STPSCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Characters/SCharacter.h"
#include "InputActionValue.h"
#include "STPSCharacter.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API ASTPSCharacter : public ASCharacter
{
	GENERATED_BODY()
	
public:
	ASTPSCharacter();

	virtual void Tick(float DeltaSeconds) override;	//FOV확대 구현

	virtual void BeginPlay() override;

	float GetForwardInputValue() const { return ForwardInputValue; }

	float GetRightInputValue() const { return RightInputValue; }

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Move(const FInputActionValue& InValue);

	void Look(const FInputActionValue& InValue);

	void Attack(const FInputActionValue& InValue);
	
	void Fire();	//단발/연발 토글 구현


	//우클릭 FOV 확대 구현
	void StartIronSight(const FInputActionValue& InValue);

	void EndIronSight(const FInputActionValue& InValue);

	//단발/연발 토글 구현
	void ToggleTrigger(const FInputActionValue& InValue);

	void StartFire(const FInputActionValue& InValue);

	void StopFire(const FInputActionValue& InValue);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess))
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess))
	TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess))
	TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess = true))
	float ForwardInputValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess = true))
	float RightInputValue;


	//FOV확대 구현
	float TargetFOV = 70.f;

	float CurrentFOV = 70.f;

	//단발/연발 토글 구현
	bool bIsTriggerToggle = false;

	UPROPERTY(EditDefaultsOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess))
	float FirePerMinute = 600;

	FTimerHandle BetweenShotsTimer;

	float TimeBetweenFire;
};
