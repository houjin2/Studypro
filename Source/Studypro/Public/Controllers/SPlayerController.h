// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    ASPlayerController();

protected:
	virtual void BeginPlay() override;
};
