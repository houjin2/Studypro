// SPlayerController.cpp


#include "Controllers/SPlayerController.h"
#include "UI/SHUD.h"
#include "Game/SPlayerState.h"
#include "Components/SStatComponent.h"
#include "Characters/SRPGCharacter.h"
#include "Blueprint//UserWidget.h"

ASPlayerController::ASPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
    //엑터의 Tick이 true여만 입력을 처리 가능
}

void ASPlayerController::ToggleMenu()
{
    if (false == bIsMenuOn)
    {
        MenuUIInstance->SetVisibility(ESlateVisibility::Visible);

        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(MenuUIInstance->GetCachedWidget());
        SetInputMode(Mode);

        // SetPause(false); 만약 게임 일시정지를 원한다면.
        bShowMouseCursor = true;    
    }
    else
    {
        MenuUIInstance->SetVisibility(ESlateVisibility::Collapsed);

        FInputModeGameOnly InputModeGameOnly;
        SetInputMode(InputModeGameOnly);

        //SetPause(false); 만약 게임 일시정지를 원하면.
        bShowMouseCursor = false;
    }

    bIsMenuOn = !bIsMenuOn;
}

void ASPlayerController::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameOnly InputModeGameOnly;   //플레이시 바로 움직일수 있게 함
    SetInputMode(InputModeGameOnly);

    if (true == ::IsValid(HUDWidgetClass))
    {
        HUDWidget = CreateWidget<USHUD>(this, HUDWidgetClass);
        if (true == ::IsValid(HUDWidget))
        {
            HUDWidget->AddToViewport();

            ASPlayerState* SPlayerState = GetPlayerState<ASPlayerState>();
            if (true == ::IsValid(SPlayerState))
            {
                HUDWidget->BindPlayerState(SPlayerState);
            }

            ASCharacter* PC = GetPawn<ASCharacter>();
            if (true == ::IsValid(PC))
            {
                USStatComponent* StatComponent = PC->GetStatComponent();
                if (true == ::IsValid(StatComponent))
                {
                    HUDWidget->BindStatComponent(StatComponent);
                }
            }
        }
    }

    //ESC메뉴 구현
    if (true == ::IsValid(MenuUIClass))
    {
        MenuUIInstance = CreateWidget<UUserWidget>(this, MenuUIClass);
        if (true == ::IsValid(MenuUIInstance))
        {
            MenuUIInstance->AddToViewport(3);   //상위에 띄움

            MenuUIInstance->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}



