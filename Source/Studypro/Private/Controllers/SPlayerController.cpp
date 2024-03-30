// SPlayerController.cpp


#include "Controllers/SPlayerController.h"
#include "UI/SHUD.h"
#include "Game/SPlayerState.h"
#include "Components/SStatComponent.h"
//#include "Characters/SRPGCharacter.h"
#include "Characters/STPSCharacter.h"
#include "Blueprint//UserWidget.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Game/SGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/SGameResultWidget.h"
#include "Components/TextBlock.h"


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

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(ThisClass, NotificationText)
}

void ASPlayerController::OnOwningCharacterDead()
{
    ASGameMode* GameMode = Cast<ASGameMode>(UGameplayStatics::GetGameMode(this));
    if (true == HasAuthority() && true == ::IsValid(GameMode))
    {
        GameMode->OnControllerDead(this);
    }
}

void ASPlayerController::ShowWinnerUI_Implementation()
{
    if (false == HasAuthority())
    {
        if (true == ::IsValid(WinnerUIClass))
        {
            USGameResultWidget* WinnerUI = CreateWidget<USGameResultWidget>(this, WinnerUIClass);
            if (true == ::IsValid(WinnerUI))
            {
                WinnerUI->AddToViewport(3);
                WinnerUI->RankingText->SetText(FText::FromString(TEXT("#01")));

                FInputModeUIOnly Mode;
                Mode.SetWidgetToFocus(WinnerUI->GetCachedWidget());
                SetInputMode(Mode);

                bShowMouseCursor = true;
            }
        }
        else
        {
            UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Not find WinnerUIClass")),true , true , FLinearColor::Green, 20.0f);
        }
    }
}

void ASPlayerController::ShowLoserUI_Implementation(int32 InRanking)
{
    if (false == HasAuthority())
    {
        if (true == ::IsValid(LoserUIClass))
        {
            USGameResultWidget* LoserUI = CreateWidget<USGameResultWidget>(this, LoserUIClass);
            if (true == ::IsValid(LoserUI))
            {
                LoserUI->AddToViewport(3);
                FString RankingString = FString::Printf(TEXT("#%02d"), InRanking);
                LoserUI->RankingText->SetText(FText::FromString(RankingString));

                FInputModeUIOnly Mode;
                Mode.SetWidgetToFocus(LoserUI->GetCachedWidget());
                SetInputMode(Mode);

                bShowMouseCursor = true;
            }
        }
        else
        {
            UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Not find LoserUIClass")), true, true, FLinearColor::Green, 20.0f);
        }
    }
}

void ASPlayerController::ReturnToLobby_Implementation()
{
    if (false == HasAuthority())    //������ ������ ���ϴ°� ������ ����. Ŭ�� �̵��ؾ� �ϹǷ� if()ó�� �ʼ�.
    {
        UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=Lobby?Saved=false")));
    }
}

void ASPlayerController::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameOnly InputModeGameOnly;   //�÷��̽� �ٷ� �����ϼ� �ְ� ��
    SetInputMode(InputModeGameOnly);


    //�� �Ʒ����ʹ� Owning Client�� ������ ���̱� ���� ����.
    //�ٸ� Client������ �� PlayerController�� ������ ���� ������ �������� �ƴ����� üũ.
    if (true == HasAuthority())
    {
        return;
    }

    if (true == ::IsValid(HUDWidgetClass))
    {
        HUDWidget = CreateWidget<USHUD>(this, HUDWidgetClass);
        if (true == ::IsValid(HUDWidget))
        {
            HUDWidget->AddToViewport();


            /*
            ASPlayerState* SPlayerState = GetPlayerState<ASPlayerState>();
            if (true == ::IsValid(SPlayerState))
            {
                HUDWidget->BindPlayerState(SPlayerState);
            }
            */

            ASCharacter* PC = GetPawn<ASCharacter>();
            if (true == ::IsValid(PC))
            {
                USStatComponent* StatComponent = PC->GetStatComponent();
                if (true == ::IsValid(StatComponent))
                {
                    HUDWidget->BindStatComponent(StatComponent);
                }
            }

            FTimerHandle TimerHandle;

            //PlayerState�� Beginplay���� �ٷ� ��������� �������� �ִ�. �׷��� ������ �ణ �ð��� �� �ڿ� PlayerState�� �������ִ� �۾�
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void {
                ASPlayerState* SPlayerState = GetPlayerState<ASPlayerState>();
                if (true == ::IsValid(SPlayerState))
                {
                    HUDWidget->BindPlayerState(SPlayerState);
                }
            }), 0.5f, false);
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

    //Crosshair ����
    if (true == ::IsValid(CrosshairUIClass))
    {
        UUserWidget* CrosshairUI = CreateWidget<UUserWidget>(this, CrosshairUIClass);
        if (true == ::IsValid(CrosshairUI))
        {
            CrosshairUI->AddToViewport(1);

            CrosshairUI->SetVisibility(ESlateVisibility::Visible);
        }
    }

    if (true == ::IsValid(NotificationTextUIClass))
    {
        UUserWidget* NotificationTextUI = CreateWidget<UUserWidget>(this, NotificationTextUIClass);
        if (true == ::IsValid(NotificationTextUI))
        {
            NotificationTextUI->AddToViewport(1);

            NotificationTextUI->SetVisibility(ESlateVisibility::Visible);
        }
    }
}



