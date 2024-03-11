// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "Engine/AssetManager.h"
#include "SGameInstance.generated.h"


/**
 * 
 */

USTRUCT(BlueprintType)
struct FSStatTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FSStatTableRow()
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxEXP;

};


UCLASS()
class STUDYPRO_API USGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	
	virtual void Init() override;

	virtual void Shutdown() override;

	const UDataTable* GetCharacterStatDataTable() { return CharacterStatDataTable; }

	FSStatTableRow* USGameInstance::GetCharacterStatDataTableRow(int32 InLevel);

	FStreamableManager StreamableManager = FStreamableManager();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "USGameInstance", meta = (AllowprivateAccess))
	class UDataTable* CharacterStatDataTable;

};
