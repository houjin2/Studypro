 // Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SUnrealObjectClass.h"
#include "Examples/SFlyable.h"
#include "Examples/SPigeon.h"
#include "JsonObjectConverter.h"	//Json颇老 历厘
#include "UObject/SavePackage.h"	//Json颇老 历厘

void USGameInstance::Init()
{
	Super::Init();

	if (false == ::IsValid(CharacterStatDataTable) || CharacterStatDataTable->GetRowMap().Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough data in CharacterStatDataTable"));
	}
	else
	{
		for (int32 i = 1; i <= CharacterStatDataTable->GetRowMap().Num(); ++i)
		{
			check(nullptr != GetCharacterStatDataTableRow(i));
		}
	}

}

void USGameInstance::Shutdown()
{
	Super::Shutdown();

}

FSStatTableRow* USGameInstance::GetCharacterStatDataTableRow(int32 InLevel)
{
	if (true == ::IsValid(CharacterStatDataTable))
	{
		return CharacterStatDataTable->FindRow<FSStatTableRow>(*FString::FromInt(InLevel), TEXT(""));
	}

	return nullptr;
}
