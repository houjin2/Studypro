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

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void OnOwningCharacterDead();

    UFUNCTION(Client, Reliable)
    void ShowWinnerUI();

    UFUNCTION(Client, Reliable)
    void ShowLoserUI(int32 InRanking);

    UFUNCTION(Client, Reliable)
    void ReturnToLobby();

protected:
	virtual void BeginPlay() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASPlayerController", Meta = (AllowprivateAccess))
    TSubclassOf<class UUserWidget> MenuUIClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASPlayerController", Meta = (AllowprivateAccess))
    TObjectPtr<class UUserWidget> MenuUIInstance;

    bool bIsMenuOn = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASPlayerController", Meta = (AllowprivateAccess))
    TSubclassOf<class UUserWidget> NotificationTextUIClass;

public:
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "ASPlayerController", Meta = (AllowprivateAccess))
    FText NotificationText;

private:
    UPROPERTY();
    TObjectPtr<class USHUD> HUDWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ASPlayerController", Meta = (AllowPrivateAccess))
    TSubclassOf<class USHUD> HUDWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASPlayerController", Meta = (AllowprivateAccess))
    TSubclassOf<class UUserWidget> CrosshairUIClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASPlayerController", Meta = (AllowprivateAccess))
    TSubclassOf<class USGameResultWidget> WinnerUIClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASPlayerController", Meta = (AllowprivateAccess))
    TSubclassOf<class USGameResultWidget> LoserUIClass;

};
