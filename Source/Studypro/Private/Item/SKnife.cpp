// SKnife.cpp


#include "Item/SKnife.h"

ASKnife::ASKnife()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WeaponMeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/MilitaryWeapSilver/Weapons/Knife_A.Knife_A'"));
	if (true == WeaponMeshAsset.Succeeded())
	{
		SkeletalMeshComponent->SetSkeletalMesh(WeaponMeshAsset.Object);
	}

	WeaponType = EWeaponType::Knife;
}
