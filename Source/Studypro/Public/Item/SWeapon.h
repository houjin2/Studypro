// SWeapon.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None = 0,
	Knife = 1,
	End
};

UCLASS()
class STUDYPRO_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	TObjectPtr<class USkeletalMeshComponent> GetMesh() const { return SkeletalMeshComponent; }

	EWeaponType GetWeaponType() const { return WeaponType; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASWeapon", Meta = (AllowprivateAccess))
	TObjectPtr<class USkeletalMeshComponent> SkeletalMeshComponent;

	EWeaponType WeaponType;

};
