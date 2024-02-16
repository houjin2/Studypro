// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SUnrealObjectClass.generated.h"
/**
 * 
 */
UCLASS()
class STUDYPRO_API USUnrealObjectClass : public UObject
{
	GENERATED_BODY()

public:
	USUnrealObjectClass();

	UFUNCTION()
	void HelloUnreal();

	const FString& GetName() const { return Name; }

public:
	UPROPERTY()
	FString Name;
};
