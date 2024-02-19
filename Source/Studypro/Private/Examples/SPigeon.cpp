// SPigeon.cpp


#include "Examples/SPigeon.h"

USPigeon::USPigeon()
{
	Name = TEXT("PigeonCDO");
	ID = 0;
}

void USPigeon::Fly()
{
	UE_LOG(LogTemp, Log, TEXT("%s is now flying."), *Name);
}

void USPigeon::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar << Name;
	Ar << ID;
}
