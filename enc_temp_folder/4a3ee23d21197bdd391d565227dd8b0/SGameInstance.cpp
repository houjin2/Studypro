 // Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SUnrealObjectClass.h"
#include "Examples/SFlyable.h"
#include "Examples/SPigeon.h"
#include "JsonObjectConverter.h"	//Json���� ����
#include "UObject/SavePackage.h"	//Json���� ����

USGameInstance::USGameInstance()
{
	UE_LOG(LogTemp, Log, TEXT("USGameInstance() has been called."));

	Name = TEXT("USGameInstance Class Default Object");
}

void USGameInstance::Init()
{
	Super::Init();
	//���� �Է�, ���, Json���� ����°�
	{
		FBirdData SrcRawData(TEXT("Pigeon1"), 17);
		UE_LOG(LogTemp, Log, TEXT("[SrcRawData] Name: %s, ID: %d"), *SrcRawData.Name, SrcRawData.ID);	//������ ������ Name, ID ���� �Է���

		const FString SavedDir = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));	//ProjectDir��ο� Saved�� ���� -> Saved���� ���
		UE_LOG(LogTemp, Log, TEXT("SavedDir : %s"), *SavedDir);

		const FString RawDataFileName(TEXT("RawData.bin"));	//����ȭ�� �ؼ� RawData.bin �� �����ϰڴ�
		FString AbsolutePathForRawData = FPaths::Combine(*SavedDir, *RawDataFileName);	//savedDir��ο� RawDataFileName�� �ٿ� ������ ���� -> ������ ��ü ��ΰ� ����
		UE_LOG(LogTemp, Log, TEXT("Relative path for saved file: %s"), *AbsolutePathForRawData);
		FPaths::MakeStandardFilename(AbsolutePathForRawData);	//�𸮾� �°� ��� ������
		UE_LOG(LogTemp, Log, TEXT("Absolute path for saved file: %s"), *AbsolutePathForRawData);

		FArchive* RawFileWriterAr = IFileManager::Get().CreateFileWriter(*AbsolutePathForRawData);	//FileWriter��ī�̺긦 ���鶧�� �̿Ͱ��� ���(IFileManager::Get().CreateFileWriter(������))
		if (nullptr != RawFileWriterAr)
		{
			*RawFileWriterAr << SrcRawData;	//�ش� ��ġ�� RawData�� �ѱ�
			RawFileWriterAr->Close();
			delete RawFileWriterAr;	//�޸� ���� �ȳ��� �ٷ� ������
			RawFileWriterAr = nullptr;
		}

		FBirdData DstRawData;
		FArchive* RawFileReaderAr = IFileManager::Get().CreateFileReader(*AbsolutePathForRawData);
		if (nullptr != RawFileReaderAr)
		{
			*RawFileReaderAr << DstRawData;	//RawFileData���� ������ ��� DstRawData�� ���� ( << ���⶧���� �򰥸��� ����)
			RawFileReaderAr->Close();
			delete RawFileReaderAr;
			RawFileReaderAr = nullptr;

			UE_LOG(LogTemp, Log, TEXT("[DstRawData] Name: %s, ID: %d"), *DstRawData.Name, DstRawData.ID)
		}

		SerializedPigeon = NewObject<USPigeon>();
		SerializedPigeon->SetName(TEXT("Pigeon76"));
		SerializedPigeon->SetID(76);
		UE_LOG(LogTemp, Log, TEXT("[SerializedPigeon] Name: %s, ID: %d"), *SerializedPigeon->GetName(), SerializedPigeon->GetID());

		const FString ObjectDataFileName(TEXT("ObjectData.bin"));
		FString AbsolutePathForObjectData = FPaths::Combine(*SavedDir, *ObjectDataFileName);
		FPaths::MakeStandardFilename(AbsolutePathForObjectData);

		TArray<uint8> BufferArray;
		FMemoryWriter MemoryWriterAr(BufferArray);
		SerializedPigeon->Serialize(MemoryWriterAr);

		TUniquePtr<FArchive> ObjectDataFileWriterAr = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*AbsolutePathForObjectData));
		if (nullptr != ObjectDataFileWriterAr)
		{
			*ObjectDataFileWriterAr << BufferArray;
			ObjectDataFileWriterAr->Close();

ObjectDataFileWriterAr = nullptr;	//delete ObjectDataFileWriterAr; �� ���� ȿ��
		}

		TArray<uint8> BufferArrayFromObjectDataFile;
		TUniquePtr<FArchive> ObjectDataFileReaderAr = TUniquePtr<FArchive>(IFileManager::Get().CreateFileReader(*AbsolutePathForObjectData));
		if (nullptr != ObjectDataFileReaderAr)
		{
			*ObjectDataFileReaderAr << BufferArrayFromObjectDataFile;
			ObjectDataFileReaderAr->Close();

			ObjectDataFileReaderAr = nullptr;
		}

		FMemoryReader MemoryReaderAr(BufferArrayFromObjectDataFile);
		USPigeon* Pigeon77 = NewObject<USPigeon>();
		Pigeon77->Serialize(MemoryReaderAr);
		UE_LOG(LogTemp, Log, TEXT("[Pigeon77] Name: %s, ID: %d"), *Pigeon77->GetName(), Pigeon77->GetID());

		//Json���� ��ġ ����
		const FString JsonFileName(TEXT("StudyjsonFile.txt"));
		FString AbsolutePathForJsonData = FPaths::Combine(*SavedDir, *JsonFileName);
		FPaths::MakeStandardFilename(AbsolutePathForJsonData);

		//Json �� �������
		TSharedRef<FJsonObject> SrcJsonObject = MakeShared<FJsonObject>();
		FJsonObjectConverter::UStructToJsonObject(SerializedPigeon->GetClass(), SerializedPigeon, SrcJsonObject);

		//
		FString JsonOutString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriterAr = TJsonWriterFactory<TCHAR>::Create(&JsonOutString);	//TSharedRef�� ����ϴ� ������ ������ �� �����ϰ� �ϱ� ���� ���
		if (true == FJsonSerializer::Serialize(SrcJsonObject, JsonWriterAr))
		{
			FFileHelper::SaveStringToFile(JsonOutString, *AbsolutePathForJsonData);	//������ ����
		}

		FString JsonInString;
		FFileHelper::LoadFileToString(JsonInString, *AbsolutePathForJsonData);
		TSharedRef<TJsonReader<TCHAR>> JsonReaderAr = TJsonReaderFactory<TCHAR>::Create(JsonInString);

		TSharedPtr<FJsonObject> DstJsonObject;
		if (true == FJsonSerializer::Deserialize(JsonReaderAr, DstJsonObject))
		{
			USPigeon* Pigeon78 = NewObject<USPigeon>();
			if (true == FJsonObjectConverter::JsonObjectToUStruct(DstJsonObject.ToSharedRef(), Pigeon78->GetClass(), Pigeon78))
			{
				UE_LOG(LogTemp, Log, TEXT("[Pigeon78] Name: %s, ID: %d"), *Pigeon78->GetName(), Pigeon78->GetID());
			}
		}
	}

	//TArray �ǽ�
	/*{
		const int32 ArraySize = 10;
		TArray<int32> IntArray;

		for (int32 i = 0; i <= ArraySize; ++i)
		{
			IntArray.Add(i);
		}

		for (int32 Element : IntArray)
		{
			static int32 i = 0;
			UE_LOG(LogTemp, Log, TEXT("[%d]: %d"), i++, Element);
		}
		UE_LOG(LogTemp, Log, TEXT("======="));

		IntArray.RemoveAll([](int32 InElement)->bool {return 0 == InElement % 2; });

		for (int32 Element : IntArray)
		{
			static int32 i = 0;
			UE_LOG(LogTemp, Log, TEXT("[%d]: %d"), i++, Element);
		}
		UE_LOG(LogTemp, Log, TEXT("======="));

		IntArray += {2, 4, 6, 8, 10};

		for (int32 Element : IntArray)
		{
			static int32 i = 0;
			UE_LOG(LogTemp, Log, TEXT("[%d]: %d"), i++, Element);
		}
		UE_LOG(LogTemp, Log, TEXT("======="));

		TArray<int32> IntArrayCompare;
		IntArrayCompare.Init(0, ArraySize);
		for (int32 Element : IntArrayCompare)
		{
			static int32 i = 0;
			UE_LOG(LogTemp, Log, TEXT("%d : %d"), i++, Element);
		}
		int32 IntArrayOriginal[] = { 1,3,5,7,9,2,4,6,8,10 };
		FMemory::Memcpy(IntArrayCompare.GetData(), IntArrayOriginal, sizeof(int32)* ArraySize);

		UE_LOG(LogTemp, Log, TEXT("IntArray == IntArrayCompare ? %d"), IntArray == IntArrayCompare);
	}*/

	//TSet �ǽ�
	/*{
		const int32 SetSize = 10;
		TSet<int32> IntSet;

		for (int32 i = 1; i <= SetSize; ++i)
		{
			IntSet.Add(i);
		}

		for (int32 Element : IntSet)
		{
			static int32 i = 0;
			UE_LOG(LogTemp, Log, TEXT("[%d]: %d"), i++, Element);
		}
		UE_LOG(LogTemp, Log, TEXT("======="));

		IntSet.Remove(2);
		IntSet.Remove(4);
		IntSet.Remove(6);
		IntSet.Remove(8);
		IntSet.Remove(10);

		for (int32 Element : IntSet)
		{
			static int32 i = 0;
			UE_LOG(LogTemp, Log, TEXT("[%d]: %d"), i++, Element);
		}
		UE_LOG(LogTemp, Log, TEXT("======"));

		IntSet.Add(2);
		IntSet.Add(4);
		IntSet.Add(6);
		IntSet.Add(8);
		IntSet.Add(10);

		for (int32 Element : IntSet)
		{
			static int32 i = 0;
			UE_LOG(LogTemp, Log, TEXT("[%d]: %d"), i++, Element);
		}
		UE_LOG(LogTemp, Log, TEXT("======"));

		int32 Key = 2;
		UE_LOG(LogTemp, Log, TEXT("%d: %s"), Key, nullptr == IntSet.Find(Key) ? TEXT("nullptr") : TEXT("is in"));

		Key = 11;
		UE_LOG(LogTemp, Log, TEXT("%d: %s"), Key, nullptr == IntSet.Find(Key) ? TEXT("nullptr") : TEXT("is in"));
	}*/

	//TMap �ǽ�
	{
		TMap<int32, FString> BirdMap;
		BirdMap.Add(5, TEXT("Pigeon"));
		BirdMap.Add(2, TEXT("Owl"));
		BirdMap.Add(7, TEXT("Albatross"));
		//BirdMap == [ { Key : 5, value : "Pigeon" }, { Key : 2, Value : Owl } .. ] �� ���� �����

		BirdMap.Add(2, TEXT("Penquin"));
		//BirdMap == [ { Key : 5, value : "Pigeon" }, { Key : 2, Value : Penquin } .. ] �� ���� �����

		FString* BirdIn7 = BirdMap.Find(7);	//BirdIn7 = Albatross
		FString* BirdIn8 = BirdMap.Find(8);	//BirdIn2 = nullptr
		UE_LOG(LogTemp, Log, TEXT("BirdIn7: %s"), **BirdIn7);
	}
}

void USGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Log, TEXT("USGameInstance::Shutdown() has been called."));
}
