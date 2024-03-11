// SCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SStatComponent.h"
#include "SCharacter.generated.h"

UCLASS()
class STUDYPRO_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASCharacter();

	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	class USStatComponent* GetStatComponent() { return StatComponent; }

	UFUNCTION()
	virtual void OnCharacterDeath();

	virtual void SetWidget(class UStudyUserWidget* InStudyUserWidget) {}
private:

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="ASCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ASPlayerCharacter, Meta = (AllowPrivateAccess))
	TObjectPtr<class USStatComponent> StatComponent;
};
