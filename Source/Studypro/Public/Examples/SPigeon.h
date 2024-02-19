// SPigeon.cpp

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFlyable.h"
#include "SPigeon.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API USPigeon 
	: public UObject
	, public ISFlyable
{
	GENERATED_BODY()
	
public:
	USPigeon();

	virtual void Fly() override;

	const FString& GetName() const { return Name; }

	void SetName(const FString& InName) { Name = InName; }

	int32 GetID() const { return ID; }

	void SetID(int32 InID) { ID = InID; }

	virtual void Serialize(FArchive& Ar) override;
				
public:
	UPROPERTY()
	FString Name;

	UPROPERTY()
	int32 ID;
};
