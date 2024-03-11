// SHUD.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SHUD.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPRO_API USHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindStatComponent(class USStatComponent* InStatComponent);

	void BindPlayerState(class ASPlayerState* InPlayerState);

protected:
	UFUNCTION()
	virtual void UpdateLevelText(int32 InOldLevel, int32 InNewLevel);

protected:
	TWeakObjectPtr<class USStatComponent> StatComponent;

	TWeakObjectPtr<class ASPlayerState> PlayerState;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "USHUD", Meta = (BindWidget))
	TObjectPtr<class UTextBlock> LevelText;
		//BindWidget�� ����Ͽ� �ϵ��ڵ����� �Ӽ��� ������ ���ε� ���ص���!!

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "USHUD", Meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "USHUD", Meta = (BindWidget))
	TObjectPtr<class USW_HPBar> HPBar;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "USHUD", Meta = (BindWidget))
	TObjectPtr<class USW_EXPBar> EXPBar;
};
