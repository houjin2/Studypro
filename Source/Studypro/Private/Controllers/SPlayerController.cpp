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
    //������ Tick�� true���� �Է��� ó�� ����
}

void ASPlayerController::ToggleMenu()
{
    if (false == bIsMenuOn)
    {
        MenuUIInstance->SetVisibility(ESlateVisibility::Visible);

        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(MenuUIInstance->GetCachedWidget());
        SetInputMode(Mode);

        // SetPause(false); ���� ���� �Ͻ������� ���Ѵٸ�.
        bShowMouseCursor = true;    
    }
    else
    {
        MenuUIInstance->SetVisibility(ESlateVisibility::Collapsed);

        FInputModeGameOnly InputModeGameOnly;
        SetInputMode(InputModeGameOnly);

        //SetPause(false); ���� ���� �Ͻ������� ���ϸ�.
        bShowMouseCursor = false;
    }

    bIsMenuOn = !bIsMenuOn;
}

void ASPlayerController::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameOnly InputModeGameOnly;   //�÷��̽� �ٷ� �����ϼ� �ְ� ��
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

    //ESC�޴� ����
    if (true == ::IsValid(MenuUIClass))
    {
        MenuUIInstance = CreateWidget<UUserWidget>(this, MenuUIClass);
        if (true == ::IsValid(MenuUIInstance))
        {
            MenuUIInstance->AddToViewport(3);   //������ ���

            MenuUIInstance->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}



