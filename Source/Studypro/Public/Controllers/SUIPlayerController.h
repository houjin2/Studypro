// SUIPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SUIPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API ASUIPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ASUIPlayerController", meta = (AllowprivateAccess))
	TSubclassOf<class UUserWidget> UIWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ASUIPlayerController", meta = (AllowprivateAccess))
	TObjectPtr<class UUserWidget> UIWidgetInstance;
};
