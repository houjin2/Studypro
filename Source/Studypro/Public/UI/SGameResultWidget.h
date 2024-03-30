// SGameResultWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SGameResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API USGameResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnReturnToLobbyButtonClicked();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USGameResultWidget", Meta = (BindWidget))
	TObjectPtr<class UTextBlock> RankingText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USGameResultWidget", Meta = (BindWidget))
	TObjectPtr<class UButton> ReturnToLobbyButton;
};
