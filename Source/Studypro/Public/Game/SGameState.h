// SGameState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None,
	Waiting,
	Playing,
	Ending,
	End
};

/**
 * 
 */
UCLASS()
class STUDYPRO_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "ASGameState")
	int32 AlivePlayerControllerCount;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "ASGameState")
	EMatchState MatchState = EMatchState::Waiting;
};
