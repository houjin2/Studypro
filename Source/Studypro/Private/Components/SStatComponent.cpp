// SStatComponent.cpp


#include "Components/SStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Game/SGameInstance.h"
#include "Game/SPlayerState.h"
#include "Characters/SRPGCharacter.h"
#include "Net/UnrealNetwork.h"      //애니메이션 동기화
#include "Engine/Engine.h"          //애니메이션 동기화

USStatComponent::USStatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bWantsInitializeComponent = false;
}

void USStatComponent::BeginPlay()
{
    Super::BeginPlay();

    GameInstance = Cast<USGameInstance>(GetWorld()->GetGameInstance());
    if (true == ::IsValid(GameInstance))
    {
        if (nullptr != GameInstance->GetCharacterStatDataTable() || nullptr != GameInstance->GetCharacterStatDataTableRow(1))
        {
            float NewMaxHP = GameInstance->GetCharacterStatDataTableRow(1)->MaxHP;
            SetMaxHP(NewMaxHP);
            SetCurrentHP(MaxHP);
        }
    }

    ASCharacter* OwnerPlayerCharacter = Cast<ASCharacter>(GetOwner());
    if (true == ::IsValid(OwnerPlayerCharacter))
    {
        ASPlayerState* PS = Cast<ASPlayerState>(OwnerPlayerCharacter->GetPlayerState());
        if (true == ::IsValid(PS))
            {
                if (false == PS->OnCurrentLevelChangedDelegate.IsAlreadyBound(this, &ThisClass::OnCurrentLevelChanged))
                {
                    PS->OnCurrentLevelChangedDelegate.AddDynamic(this, &ThisClass::OnCurrentLevelChanged);
                }
            }
    }
}

void USStatComponent::SetMaxHP(float InMaxHP)
{
    if (true == OnMaxHPChangeDelegate.IsBound())
    {
        OnMaxHPChangeDelegate.Broadcast(MaxHP, InMaxHP);
    }

    MaxHP = FMath::Clamp<float>(InMaxHP, 0.f, 9999);
}

void USStatComponent::SetCurrentHP(float InCurrentHP)
{
    if (true == OnCurrentHPChangeDelegate.IsBound())
    {
        OnCurrentHPChangeDelegate.Broadcast(CurrentHP, InCurrentHP);
    }

    CurrentHP = FMath::Clamp<float>(InCurrentHP, 0.f, MaxHP);

    if (CurrentHP < KINDA_SMALL_NUMBER)
    {
        OnOutOfCurrentHPDelegate.Broadcast();
        CurrentHP = 0.f;
    }

    OnCurrentHPChanged_NetMulticast(CurrentHP, CurrentHP);
}

void USStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, MaxHP);
    DOREPLIFETIME(ThisClass, CurrentHP);
}

void USStatComponent::OnCurrentHPChanged_NetMulticast_Implementation(float InOldCurrentHP, float InNewCurrentHP)
{
    if (true == OnCurrentHPChangeDelegate.IsBound())
    {
        OnCurrentHPChangeDelegate.Broadcast(InOldCurrentHP, InNewCurrentHP);
    }

    if (InNewCurrentHP < KINDA_SMALL_NUMBER)
    {
        OnOutOfCurrentHPDelegate.Broadcast();
    }
}

void USStatComponent::OnCurrentLevelChanged(int32 InOldCurrentLevel, int32 InNewCurrentLevel)
{
    SetMaxHP(GameInstance->GetCharacterStatDataTableRow(InNewCurrentLevel)->MaxHP);
    SetCurrentHP(GameInstance->GetCharacterStatDataTableRow(InNewCurrentLevel)->MaxHP);
}



