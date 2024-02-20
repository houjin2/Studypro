// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SFlyable.generated.h"

USTRUCT()
struct FBirdData
{
	GENERATED_BODY()

public:
	FBirdData() {}

	FBirdData(const FString& InName, int32 InID)
		:Name(InName)
		,ID(InID)
	{
	}

	friend FArchive& operator<<(FArchive& Ar, FBirdData& InBirdData)
	{
		Ar << InBirdData.Name;
		Ar << InBirdData.ID;
		return Ar;
	}

	bool operator==(const FBirdData& InBirdData) const	//TMap�� �����Ҷ��� operator==�Լ��� GetTypeHash�Լ��� �־����.
	{
		return ID == InBirdData.ID;
	}

	friend uint32 GetTypeHash(const FBirdData& InBirdData)
	{
		return GetTypeHash(InBirdData.ID);
	}

	UPROPERTY()
	FString Name = TEXT("DefaultBirdName");
	int32 ID = 0;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USFlyable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STUDYPRO_API ISFlyable
{
	GENERATED_BODY()

public:
	virtual void Fly() = 0;  //�����ص� �ǰ� ���ص� ��. ���ϴ°� ����
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
};
