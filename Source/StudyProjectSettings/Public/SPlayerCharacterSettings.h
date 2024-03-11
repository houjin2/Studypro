// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SPlayerCharacterSettings.generated.h"

/**
 * 
 */
UCLASS(config = PlayerCharacterMeshPaths)	// 언리얼 엔진의 초기화 단계에서 config폴더에 위치한 DefaultPlayerCharacterMeshPaths.ini 파일을 읽어들임.
class STUDYPROJECTSETTINGS_API USPlayerCharacterSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config)	// 읽어들인 DefaultPlayerCharacterMeshPaths.ini 파일의 내용으로 해당 멤버의 기본값이 초기화됨.
		TArray<FSoftObjectPath> PlayerCharacterMeshPaths;
};
