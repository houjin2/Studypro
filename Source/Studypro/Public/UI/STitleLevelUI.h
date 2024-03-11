// STitleLevelUI.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STitleLevelUI.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API USTitleLevelUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	USTitleLevelUI(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnNewGameButtonClicked();

	UFUNCTION()
	void OnExitGameButtonClicked();

	UFUNCTION()
	void OnSavedGameButtonClicked();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USTitleWidget", Meta = (AllowprivateAccess, BindWidget))
	TObjectPtr<class UButton> NewGameButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USTitleWidget", Meta = (AllowprivateAccess, BindWidget))
	TObjectPtr<class UButton> ExitGameButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USTitleWidget", Meta = (AllowprivateAccess, BindWidget))
	TObjectPtr<class UButton> SavedGameButton;
};
