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

	virtual void Tick(float DeltaSeconds) override;	//FOVȮ�� ����

	virtual void BeginPlay() override;

	float GetForwardInputValue() const { return ForwardInputValue; }

	float GetRightInputValue() const { return RightInputValue; }

	float GetCurrentAimPitch() const { return CurrentAimPitch; }

	float GetCurrentAimYaw() const { return CurrentAimYaw; }

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	//UPROPERTY Replicate�ϱ� ����
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Move(const FInputActionValue& InValue);

	void Look(const FInputActionValue& InValue);

	void Attack(const FInputActionValue& InValue);
	
	void Fire();	//�ܹ�/���� ��� ����


	//��Ŭ�� FOV Ȯ�� ����
	void StartIronSight(const FInputActionValue& InValue);

	void EndIronSight(const FInputActionValue& InValue);

	//�ܹ�/���� ��� ����
	void ToggleTrigger(const FInputActionValue& InValue);

	void StartFire(const FInputActionValue& InValue);

	void StopFire(const FInputActionValue& InValue);

	void SpawnLandMine(const FInputActionValue& InValue);

	//�κ� ������ ����Ͽ� �ǰݸ�� ��ü
	UFUNCTION()
	void OnHittedRagdollRestoreTimerElapsed();

	//Server : �ش� �Լ��� OwningClient�� ȣ���ϰ� Server���� ����(�������� ����(Validation)���� ������ Ŭ�󿡼� ������ ����.) Server or OwningClient	OwningClient = ĳ���Ͱ� ��������� UI�� ���ٴ��� �Ҷ� �ַ� ���. UI�� �ش� Client���� �ְ� Server�� OtherClient���� ����
	//Reliable : RPC�� Remote Machine���� Ȯ���� ����ǰԲ� �ϴ� Ű����. �⺻ ���� Unreliable. (���� ���� ���� Unreliable) Reliable or Unreliable
	//WithValidation : �������� ������ ������ ���ɼ��� ����. �ش� Ű���带 ����ϸ� �����Լ� �ܿ� �Ǵٸ� ������ �Լ��� ���� ����. �Ǵٸ� �Լ��� �ý��� ���� �˸� ���� �ۼ� ����. 
	UFUNCTION(Server, Reliable, WithValidation)	 
	void SpawnLandMine_Server();


	//�ִϸ��̼� ����ȭ
	UFUNCTION(Server, Unreliable)
	void UpdateInputValue_Server(const float& InForwardInputValue, const float& InRightInputValue);

	UFUNCTION(Server, Unreliable)
	void UpdateAimValue_Server(const float& InAimPitchValue, const float& InAimYawValue);

	UFUNCTION(Server, Unreliable)
	void PlayAttackMontage_Server();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayAttackMontage_NetMulticast();

	//������ ����ȭ
	//���� Ʈ���̽��� �ٸ� Ŭ�󿡼��� �����ϸ� ���������� ������ ���� �� ����. ���� ����Ʈ���̽� ������ Owning Client������ ����ǰԲ� �ϴ°� �ùٸ�. TakeDamage() �Լ��� Server������ ȣ��Ǵ� ���� �ùٸ�.
	//�Ѿ� ������ NetMulticast�� ���� �� Ŭ�󿡼� DrawDebugLine()�� ȣ���ϴ°� ����.
	UFUNCTION(Server, Reliable)
	void ApplyDamageAndDrawLine_Server(const FVector& InDrawStart, const FVector& InDrawEnd, class ACharacter* InHittedCharacter, float InDamage, struct FDamageEvent const& InDamageEvent, AController* InEventInstigator, AActor* InDamageCauser);

	UFUNCTION(NetMulticast, Reliable)
	void DrawLine_NetMulticast(const FVector& InDrawStart, const FVector& InDrawEnd);

	//�ǰ� ��� ����ȭ
    UFUNCTION(NetMulticast, Unreliable)
    void PlayRagdoll_NetMulticast();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess))
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess))
	TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess))
	TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", meta = (AllowPrivateAccess = true))
	float ForwardInputValue;

	float PreviousForwardInputValue = 0.f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", meta = (AllowPrivateAccess = true))
	float RightInputValue;

	float PreviousRightInputValue = 0.f;

	//FOVȮ�� ����
	float TargetFOV = 70.f;

	float CurrentFOV = 70.f;

	//�ܹ�/���� ��� ����
	bool bIsTriggerToggle = false;

	UPROPERTY(EditDefaultsOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess))
	float FirePerMinute = 600;

	FTimerHandle BetweenShotsTimer;

	float TimeBetweenFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowprivateAccess = true))
	TObjectPtr<class UAnimMontage> RifleFireAnimMontage;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowprivateAccess = true))
	TSubclassOf<class UCameraShakeBase> FireShake;


	//���� �ǰݸ�� ����
	FTimerHandle HittedRagdollRestoreTimer;

	FTimerDelegate HittedRagdollRestoreTimerDelegate;

	float TargetRagdollBlendWeight = 0.f;

	float CurrentRagdollBlendWeight = 0.f;

	bool bIsNowRagdollBlending = false;


	//LandMine
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASRPSCharacter", Meta = (AllowprivateAccess = true));
	TSubclassOf<class AActor> LandMineClass;


	//�ִϸ��̼� ����ȭ
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowprivateAccess = true))
	float CurrentAimPitch = 0.f;

	float PreviousAimPitch = 0.f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowprivateAccess = true))
	float CurrentAimYaw = 0.f;

	float PreviousAimYaw = 0.f;

};
