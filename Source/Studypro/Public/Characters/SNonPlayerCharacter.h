// SNonPlayerCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Characters/SCharacter.h"
#include "SNonPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API ASNonPlayerCharacter : public ASCharacter
{
	GENERATED_BODY()
	
public:
	ASNonPlayerCharacter();

	virtual void BeginPlay() override;
};
