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

	USObject1->HelloUnreal();//이렇게 호출할수도 있겠지만

	UFunction* HelloUnrealFunction = USObject1->GetClass()->FindFunctionByName(TEXT("HelloUnreal"));
	if (nullptr != HelloUnrealFunction)
	{
		USObject1->ProcessEvent(HelloUnrealFunction, nullptr);
	}
	//더 확실하게 해당 함수가 있으면 호출할수 있게 만듦
}

void USGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Log, TEXT("USGameInstance::Shutdown() has been called."));
}
