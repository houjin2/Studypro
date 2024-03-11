// SPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    ASPlayerController();

	class USHUD* GetHUDWidget() const { return HUDWidget; };

    void ToggleMenu();

protected:
	virtual void BeginPlay() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASPlayerController", Meta = (AllowprivateAccess))
    TSubclassOf<class UUserWidget> MenuUIClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASPlayerController", Meta = (AllowprivateAccess))
    TObjectPtr<class UUserWidget> MenuUIInstance;

    bool bIsMenuOn = false;

private:
    UPROPERTY();
    TObjectPtr<class USHUD> HUDWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ASPlayerController", Meta = (AllowPrivateAccess));
    TSubclassOf<class USHUD> HUDWidgetClass;
};
