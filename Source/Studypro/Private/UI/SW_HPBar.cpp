// SW_HPBar.cpp


#include "UI/SW_HPBar.h"
#include "Components/ProgressBar.h"
#include "Characters/SCharacter.h"
#include "Components/SStatComponent.h"

void USW_HPBar::SetMaxHP(float InMaxHP)
{
    SetMaxFigure(InMaxHP);
}

void USW_HPBar::InitializeHPBarWidget(USStatComponent* NewStatComponent)
{
    OnCurrentHPChange(0, NewStatComponent->GetCurrentHP());
}

void USW_HPBar::OnMaxHPChange(float InOldMaxHP, float InNewMaxHP)
{
    SetMaxFigure(InNewMaxHP);

    OnCurrentHPChange(0, InNewMaxHP);
}

void USW_HPBar::OnCurrentHPChange(float InOldHP, float InNewHP)
{
    if (true == ::IsValid(Bar))
    {
        if (KINDA_SMALL_NUMBER < MaxFigure)
        {
            Bar->SetPercent(InNewHP / MaxFigure);
        }
        else
        {
            Bar->SetPercent(0.f);
        }
    }
}

void USW_HPBar::NativeConstruct()
{
    Super::NativeConstruct();

    ASCharacter* OwningCharacter = Cast<ASCharacter>(OwningActor);
    if (true == ::IsValid(OwningCharacter))
    {
        OwningCharacter->SetWidget(this);   //UserWidget은 OwningActor를 알지만 OwningActor는 UserWidget이 뭔지 모를수 있기 때문에 SetWidget을 만들어 호출해줌.
    }
}