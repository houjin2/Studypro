// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SPlayerCharacterSettings.generated.h"

/**
 * 
 */
UCLASS(config = PlayerCharacterMeshPaths)	// �𸮾� ������ �ʱ�ȭ �ܰ迡�� config������ ��ġ�� DefaultPlayerCharacterMeshPaths.ini ������ �о����.
class STUDYPROJECTSETTINGS_API USPlayerCharacterSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config)	// �о���� DefaultPlayerCharacterMeshPaths.ini ������ �������� �ش� ����� �⺻���� �ʱ�ȭ��.
		TArray<FSoftObjectPath> PlayerCharacterMeshPaths;
};
