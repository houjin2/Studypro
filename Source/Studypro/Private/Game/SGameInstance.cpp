 // Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SUnrealObjectClass.h"

USGameInstance::USGameInstance()
{
	UE_LOG(LogTemp, Log, TEXT("USGameInstance() has been called."));

	Name = TEXT("USGameInstance Class Default Object");
}

void USGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("USGameInstance::Init() has been called."));

	//UKismetSystemLibrary::PrintString(this, TEXT("PrintString() has been called."));

	USUnrealObjectClass* USObject1 = NewObject<USUnrealObjectClass>();

	UE_LOG(LogTemp, Log, TEXT("USObject1 Name : %s"), *USObject1->GetName());

	FProperty* NameProperty = USUnrealObjectClass::StaticClass()->FindPropertyByName(TEXT("Name"));

	FString CompileTimeUSObjectName;

	if (nullptr != NameProperty)
	{
		NameProperty->GetValue_InContainer(USObject1, &CompileTimeUSObjectName);
		UE_LOG(LogTemp, Log, TEXT("CompileTimeUSObjectName: %s"), *CompileTimeUSObjectName);
	}

	USObject1->HelloUnreal();//�̷��� ȣ���Ҽ��� �ְ�����

	UFunction* HelloUnrealFunction = USObject1->GetClass()->FindFunctionByName(TEXT("HelloUnreal"));
	if (nullptr != HelloUnrealFunction)
	{
		USObject1->ProcessEvent(HelloUnrealFunction, nullptr);
	}
	//�� Ȯ���ϰ� �ش� �Լ��� ������ ȣ���Ҽ� �ְ� ����
}

void USGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Log, TEXT("USGameInstance::Shutdown() has been called."));
}
