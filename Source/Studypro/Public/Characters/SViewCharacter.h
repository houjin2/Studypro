// SViewCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Characters/SCharacter.h"
#include "InputActionValue.h"
#include "SViewCharacter.generated.h"

UENUM()
enum class EViewMode : uint8	//Backview 구현
{
	None,	//enum class 를 만들때 항상 None 하나 붙여줌 -> 초기화할때 사용
	Backview,
	QuarterView,
	End		//End는  enum class에 있는 종류 갯수를 의미
};

UCLASS()
class STUDYPRO_API ASViewCharacter : public ASCharacter
{
	GENERATED_BODY()
	
public:
	ASViewCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PossessedBy(AController* NewController) override;	//Controller에 의해 빙의될때 해당 함수 사용됨

	void SetViewMode(EViewMode InViewMode);

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Move(const FInputActionValue& InValue);

	void Look(const FInputActionValue& InValue);

	void ChangeView(const FInputActionValue& InValue);

private:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SViewCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "SViewCharacter", Meta = (AllowprivateAccess))
	TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

	EViewMode CurrentViewMode = EViewMode::None;	//UPROPERTY() 매크로를 사용하지 않으므로 초기화에 유념. UPROPERTY()를 사용해야 초기화를 안해줄수 있음

	FVector DirectionToMove = FVector::ZeroVector;

	float DestArmLength = 0.f;

	float ArmLengthChangeSpeed = 3.f;

	FRotator DestArmRotation = FRotator::ZeroRotator;

	float ArmRotationChangeSpeed = 10.f;

};
