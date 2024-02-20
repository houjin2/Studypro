 // Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SUnrealObjectClass.h"
#include "Examples/SFlyable.h"
#include "Examples/SPigeon.h"
#include "JsonObjectConverter.h"	//Json파일 저장
#include "UObject/SavePackage.h"	//Json파일 저장

USGameInstance::USGameInstance()
{
	UE_LOG(LogTemp, Log, TEXT("USGameInstance() has been called."));

	Name = TEXT("USGameInstance Class Default Object");
}

void USGameInstance::Init()
{
	Super::Init();
	//파일 입력, 출력, Json파일 만드는것
	{
		FBirdData SrcRawData(TEXT("Pigeon1"), 17);
		UE_LOG(LogTemp, Log, TEXT("[SrcRawData] Name: %s, ID: %d"), *SrcRawData.Name, SrcRawData.ID);	//데이터 만든후 Name, ID 값을 입력함

		const FString SavedDir = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));	//ProjectDir경로에 Saved를 붙임 -> Saved폴더 경로
		UE_LOG(LogTemp, Log, TEXT("SavedDir : %s"), *SavedDir);

		const FString RawDataFileName(TEXT("RawData.bin"));	//직렬화를 해서 RawData.bin 에 저장하겠다
		FString AbsolutePathForRawData = FPaths::Combine(*SavedDir, *RawDataFileName);	//savedDir경로에 RawDataFileName을 붙여 절대경로 저장 -> 파일의 전체 경로가 나옴
		UE_LOG(LogTemp, Log, TEXT("Relative path for saved file: %s"), *AbsolutePathForRawData);
		FPaths::MakeStandardFilename(AbsolutePathForRawData);	//언리얼에 맞게 경로 재조정
		UE_LOG(LogTemp, Log, TEXT("Absolute path for saved file: %s"), *AbsolutePathForRawData);

		FArchive* RawFileWriterAr = IFileManager::Get().CreateFileWriter(*AbsolutePathForRawData);	//FileWriter아카이브를 만들때는 이와같이 사용(IFileManager::Get().CreateFileWriter(절대경로))
		if (nullptr != RawFileWriterAr)
		{
			*RawFileWriterAr << SrcRawData;	//해당 위치에 RawData를 넘김
			RawFileWriterAr->Close();
			delete RawFileWriterAr;	//메모리 누수 안나게 바로 지워줌
			RawFileWriterAr = nullptr;
		}

		FBirdData DstRawData;
		FArchive* RawFileReaderAr = IFileManager::Get().CreateFileReader(*AbsolutePathForRawData);
		if (nullptr != RawFileReaderAr)
		{
			*RawFileReaderAr << DstRawData;	//RawFileData에서 정보를 끌어내 DstRawData에 저장 ( << 방향때문에 헷갈리지 말길)
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

ObjectDataFileWriterAr = nullptr;	//delete ObjectDataFileWriterAr; 와 같은 효과
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

		//Json파일 위치 저장
		const FString JsonFileName(TEXT("StudyjsonFile.txt"));
		FString AbsolutePathForJsonData = FPaths::Combine(*SavedDir, *JsonFileName);
		FPaths::MakeStandardFilename(AbsolutePathForJsonData);

		//Json 값 만들어줌
		TSharedRef<FJsonObject> SrcJsonObject = MakeShared<FJsonObject>();
		FJsonObjectConverter::UStructToJsonObject(SerializedPigeon->GetClass(), SerializedPigeon, SrcJsonObject);

		//
		FString JsonOutString;
		TSharedRef<TJsonWriter<TCHAR>> JsonWriterAr = TJsonWriterFactory<TCHAR>::Create(&JsonOutString);	//TSharedRef를 사용하는 이유는 공유를 더 안전하게 하기 위해 사용
		if (true == FJsonSerializer::Serialize(SrcJsonObject, JsonWriterAr))
		{
			FFileHelper::SaveStringToFile(JsonOutString, *AbsolutePathForJsonData);	//데이터 저장
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

	//TArray 실습
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

	//TSet 실습
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

	//TMap 실습
	{
		TMap<int32, FString> BirdMap;
		BirdMap.Add(5, TEXT("Pigeon"));
		BirdMap.Add(2, TEXT("Owl"));
		BirdMap.Add(7, TEXT("Albatross"));
		//BirdMap == [ { Key : 5, value : "Pigeon" }, { Key : 2, Value : Owl } .. ] 와 같이 저장됨

		BirdMap.Add(2, TEXT("Penquin"));
		//BirdMap == [ { Key : 5, value : "Pigeon" }, { Key : 2, Value : Penquin } .. ] 와 같이 저장됨

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
