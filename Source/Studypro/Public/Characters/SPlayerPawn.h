//SPlayerPawn.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SPlayerPawn.generated.h"

UCLASS()
class STUDYPRO_API ASPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
    ASPlayerPawn();
    // 언리얼의 Construction Script에 따라, CDO 생성 시점임. 우리가 생각하는 개체 생성 시점과 조금 다름. 

protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void BeginPlay() override;
private:
    void UpDown(float InAxisValue);

    void LeftRight(float InAxisValue);


private:
    UPROPERTY(EditAnywhere, Category = "ASPlayerPawn", Meta = (AllowPrivateAccess))
    TObjectPtr<class UCapsuleComponent> CapsuleComponent;

    UPROPERTY(EditAnywhere, Category = "ASPlayerPawn", Meta = (AllowPrivateAccess))
    TObjectPtr<class USkeletalMeshComponent> SkeletalMeshComponent;

    UPROPERTY(EditAnywhere, Category = "ASPlayerPawn", Meta = (AllowPrivateAccess))
    TObjectPtr<class UFloatingPawnMovement> FloatingPawnMovementComponent;

    UPROPERTY(EditAnywhere, Category = "ASPlayerPawn", Meta = (AllowPrivateAccess))
    TObjectPtr<class USpringArmComponent> SpringArmComponent;

    UPROPERTY(EditAnywhere, Category = "ASPlayerPawn", Meta = (AllowPrivateAccess))
    TObjectPtr<class UCameraComponent> CameraComponent;


};
