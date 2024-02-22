// SViewCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Characters/SCharacter.h"
#include "InputActionValue.h"
#include "SViewCharacter.generated.h"

UENUM()
enum class EViewMode : uint8	//Backview ����
{
	None,	//enum class �� ���鶧 �׻� None �ϳ� �ٿ��� -> �ʱ�ȭ�Ҷ� ���
	Backview,
	QuarterView,
	End		//End��  enum class�� �ִ� ���� ������ �ǹ�
};

UCLASS()
class STUDYPRO_API ASViewCharacter : public ASCharacter
{
	GENERATED_BODY()
	
public:
	ASViewCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PossessedBy(AController* NewController) override;	//Controller�� ���� ���ǵɶ� �ش� �Լ� ����

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

	EViewMode CurrentViewMode = EViewMode::None;	//UPROPERTY() ��ũ�θ� ������� �����Ƿ� �ʱ�ȭ�� ����. UPROPERTY()�� ����ؾ� �ʱ�ȭ�� �����ټ� ����

	FVector DirectionToMove = FVector::ZeroVector;

	float DestArmLength = 0.f;

	float ArmLengthChangeSpeed = 3.f;

	FRotator DestArmRotation = FRotator::ZeroRotator;

	float ArmRotationChangeSpeed = 10.f;

};
