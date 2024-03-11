// SLobbyLevelUI.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/SPlayerStateSave.h"
#include "SLobbyLevelUI.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API USLobbyLevelUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnRedTeamButtonClicked();

	UFUNCTION()
	void OnBlueTeamButtonClicked();

	UFUNCTION()
	void OnSubmitButtonClicked();

	void SaveInitializedSaveData();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USLobbyLevelUI", Meta = (AllowprivateAccess, BindWidget))
	TObjectPtr<class UButton> RedTeamButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USLobbyLevelUI", Meta = (AllowprivateAccess, BindWidget))
	TObjectPtr<class UButton> BlueTeamButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USLobbyLevelUI", Meta = (AllowprivateAccess, BindWidget))
	TObjectPtr<class UEditableText> EditPlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USLobbyLevelUI", Meta = (AllowprivateAccess, BindWidget))
	TObjectPtr<class UButton> SubmitButton;

	FString PlayerName = TEXT("DefaultPlayerName");

	ETeamType SelectedTeamType = ETeamType::Red;

	TWeakObjectPtr<class USkeletalMeshComponent> CurrentSkeletalMeshComponent;
};
