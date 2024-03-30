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

	float GetCurrentAimPitch() const { return CurrentAimPitch; }

	float GetCurrentAimYaw() const { return CurrentAimYaw; }

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	//UPROPERTY Replicate하기 위함
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	void SpawnLandMine(const FInputActionValue& InValue);

	//부분 렉돌을 사용하여 피격모션 대체
	UFUNCTION()
	void OnHittedRagdollRestoreTimerElapsed();

	//Server : 해당 함수를 OwningClient가 호출하고 Server에서 수행(서버에서 검증(Validation)하지 않으면 클라에서 위변조 가능.) Server or OwningClient	OwningClient = 캐릭터가 사망헀을때 UI를 띄운다던가 할때 주로 사용. UI는 해당 Client에만 있고 Server와 OtherClient에는 없음
	//Reliable : RPC가 Remote Machine에서 확실히 실행되게끔 하는 키워드. 기본 값은 Unreliable. (사운드 같은 것은 Unreliable) Reliable or Unreliable
	//WithValidation : 검증하지 않으면 위변조 가능성이 있음. 해당 키워드를 사용하면 구현함수 외에 또다른 검증용 함수를 정의 가능. 또다른 함수에 시스템 차단 알림 로직 작성 가능. 
	UFUNCTION(Server, Reliable, WithValidation)	 
	void SpawnLandMine_Server();


	//애니메이션 동기화
	UFUNCTION(Server, Unreliable)
	void UpdateInputValue_Server(const float& InForwardInputValue, const float& InRightInputValue);

	UFUNCTION(Server, Unreliable)
	void UpdateAimValue_Server(const float& InAimPitchValue, const float& InAimYawValue);

	UFUNCTION(Server, Unreliable)
	void PlayAttackMontage_Server();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayAttackMontage_NetMulticast();

	//데미지 동기화
	//라인 트레이스를 다른 클라에서도 수행하면 오차때문에 문제가 생길 수 있음. 따라서 라인트레이스 로직은 Owning Client에서만 진행되게끔 하는게 올바름. TakeDamage() 함수는 Server에서만 호출되는 것이 올바름.
	//총알 궤적은 NetMulticast를 통해 각 클라에서 DrawDebugLine()을 호출하는게 맞음.
	UFUNCTION(Server, Reliable)
	void ApplyDamageAndDrawLine_Server(const FVector& InDrawStart, const FVector& InDrawEnd, class ACharacter* InHittedCharacter, float InDamage, struct FDamageEvent const& InDamageEvent, AController* InEventInstigator, AActor* InDamageCauser);

	UFUNCTION(NetMulticast, Reliable)
	void DrawLine_NetMulticast(const FVector& InDrawStart, const FVector& InDrawEnd);

	//피격 모션 동기화
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

	//FOV확대 구현
	float TargetFOV = 70.f;

	float CurrentFOV = 70.f;

	//단발/연발 토글 구현
	bool bIsTriggerToggle = false;

	UPROPERTY(EditDefaultsOnly, Category = "ASTPSCharacter", Meta = (AllowPrivateAccess))
	float FirePerMinute = 600;

	FTimerHandle BetweenShotsTimer;

	float TimeBetweenFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowprivateAccess = true))
	TObjectPtr<class UAnimMontage> RifleFireAnimMontage;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowprivateAccess = true))
	TSubclassOf<class UCameraShakeBase> FireShake;


	//렉돌 피격모션 구현
	FTimerHandle HittedRagdollRestoreTimer;

	FTimerDelegate HittedRagdollRestoreTimerDelegate;

	float TargetRagdollBlendWeight = 0.f;

	float CurrentRagdollBlendWeight = 0.f;

	bool bIsNowRagdollBlending = false;


	//LandMine
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASRPSCharacter", Meta = (AllowprivateAccess = true));
	TSubclassOf<class AActor> LandMineClass;


	//애니메이션 동기화
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowprivateAccess = true))
	float CurrentAimPitch = 0.f;

	float PreviousAimPitch = 0.f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", Meta = (AllowprivateAccess = true))
	float CurrentAimYaw = 0.f;

	float PreviousAimYaw = 0.f;

};
