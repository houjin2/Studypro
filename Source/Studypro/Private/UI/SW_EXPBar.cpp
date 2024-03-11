// SW_EXPBar.cpp


#include "UI/SW_EXPBar.h"
#include "Game/SPlayerState.h"
#include "Characters/SNonPlayerCharacter.h"
#include "Components/ProgressBar.h"

void USW_EXPBar::SetMaxEXP(float InMaxEXP)
{
	SetMaxFigure(InMaxEXP);
}

void USW_EXPBar::OnCurrentEXPChange(float InOldEXP, float InNewEXP)
{
	if (true == ::IsValid(Bar))
	{
		if (KINDA_SMALL_NUMBER < MaxFigure)
		{
			Bar->SetPercent(InNewEXP / MaxFigure);
		}
		else
		{
			Bar->SetPercent(0.f);
		}
	}
}

void USW_EXPBar::InitializeEXPBarWidget(ASPlayerState* NewPlayerState)
{
	OnCurrentEXPChange(0, NewPlayerState->GetCurrentEXP());
}
