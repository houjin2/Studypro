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

	friend class UAN_checkHit;
	
public:
	ASRPGCharacter();

	virtual void BeginPlay() override;

	float GetForwardInputValue() const { return ForwardInputValue; }

	float GetRightInputValue() const { return RightInputValue; }

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Move(const FInputActionValue& InValue);

	void Look(const FInputActionValue& InValue);

	void Attack(const FInputActionValue& InValue);

	UFUNCTION()
	void CheckHit();

	void BeginCombo();

	UFUNCTION()
	void CheckCanNextCombo();

	UFUNCTION()
	void EndCombo(class UAnimMontage* InAnimMontage, bool bInterrupted);
private:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SRPGCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SRPGCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SRPGCharacter", Meta = (AllowprivateAccess = true))
	float ForwardInputValue;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SRPGCharacter", Meta = (AllowprivateAccess = true))
	float RightInputValue;

	uint8 bIsAttacking : 1;

	FString AttackAnimMontageSectionName = FString(TEXT("Attack"));

	int32 MaxComboCount = 3;

	int32 CurrentComboCount = 0;

	bool bIsAttackKeyPressed = false;	// �����Ϳ��� �����ǰų� �ø�������� �� �ʿ� �����Ƿ� �׳� bool �ڷ��� ���.

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SRPGCharacter", Meta = (AllowprivateAccess = true))
	float AttackRange = 200.f;

	float AttackRadius = 50.f;
};
