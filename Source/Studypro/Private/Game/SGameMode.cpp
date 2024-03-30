// SGameMode.cpp

#include "Game/SGameMode.h"
#include "Controllers/SPlayerController.h"
#include "Characters/SPlayerPawn.h"
#include "Game/SPlayerState.h"
#include "Game/SGameState.h"
#include "Kismet/GameplayStatics.h"

ASGameMode::ASGameMode()
{
	PlayerControllerClass = ASPlayerController::StaticClass();
	DefaultPawnClass = ASPlayerPawn::StaticClass();
}

void ASGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ASGameState* SGameState = GetGameState<ASGameState>();
	if (false == SGameState)
	{
		return;
	}

	if (SGameState->MatchState != EMatchState::Waiting)	//�� ����� ������ �ƴ�. Waiting�϶� ������ ���ϰ� ����
	{
		NewPlayer->SetLifeSpan(0.1f);	//�ٷ� Destroy�ϰԵǸ� ������ �߻��Ͽ� 0.1�� ���� ��.
		return;
	}

	ASPlayerState* PlayerState = NewPlayer->GetPlayerState<ASPlayerState>();
	if (true == ::IsValid(PlayerState))
	{
		PlayerState->InitPlayerState();
	}

	ASPlayerController* NewPlayerController = Cast<ASPlayerController>(NewPlayer);
	if (true == ::IsValid(NewPlayerController))
	{
		AlivePlayerControllers.Add(NewPlayerController);
		NewPlayerController->NotificationText = FText::FromString(TEXT("Connected to the Server."));
	}
}

void ASGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ASPlayerController* ExitingPlayerController = Cast<ASPlayerController>(Exiting);
	if (true == ::IsValid(ExitingPlayerController) && INDEX_NONE != AlivePlayerControllers.Find(ExitingPlayerController))
	{
		AlivePlayerControllers.Remove(ExitingPlayerController);
		DeadPlayerControllers.Add(ExitingPlayerController);
	}
}

void ASGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);	//1�ʸ��� �ݺ������� OnMainTimerElapsed()�� ȣ��.

	RemainWaitingTimeForPlaying = WaitingTime;
	RemainWaitingTimeForEnding = EndingTime;
}

void ASGameMode::OnControllerDead(ASPlayerController* InDeadController)
{
	if (false == ::IsValid(InDeadController) || INDEX_NONE == AlivePlayerControllers.Find(InDeadController))
	{
		return;
	}

	InDeadController->ShowLoserUI(AlivePlayerControllers.Num());

	AlivePlayerControllers.Remove(InDeadController);
	DeadPlayerControllers.Add(InDeadController);
}

void ASGameMode::OnMainTimerElapsed()
{
	ASGameState* SGameState = GetGameState<ASGameState>();
	if (false == SGameState)
	{
		return;
	}


	switch (SGameState->MatchState)
	{
	case EMatchState::None:
		break;
	case EMatchState::Waiting:
	{
		FString NotificationString = FString::Printf(TEXT(""));

		if (AlivePlayerControllers.Num() < MinimumPlayerCountForPlaying)
		{
			NotificationString = FString::Printf(TEXT("Wait another player for playing."));

			RemainWaitingTimeForPlaying = WaitingTime;	//�ּ� �ο��� �ȵǸ� ���ð� �ʱ�ȭ
		}
		else
		{
			NotificationString = FString::Printf(TEXT("Wait %d seconds for playing."), RemainWaitingTimeForPlaying);

			--RemainWaitingTimeForPlaying;
		}

		if (0 == RemainWaitingTimeForPlaying)
		{
			NotificationString = FString::Printf(TEXT(""));

			SGameState->MatchState = EMatchState::Playing;
		}

		NotifyToAllPlayer(NotificationString);

		break;
	}
	case EMatchState::Playing:
	{
		/*
		//FString NotificationString = FString::Printf(TEXT("%d / %d"), AlivePlayerControllers.Num(), AlivePlayerControllers.Num() + DeadPlayerControllers.Num());

		NotifyToAllPlayer(NotificationString);

		if (AlivePlayerControllers.Num() <= 1)
		{
			MatchState = EMatchState::Ending;
		}
		*/

		if (true == ::IsValid(SGameState))
		{
			SGameState->AlivePlayerControllerCount = AlivePlayerControllers.Num();

			FString NotificationString = FString::Printf(TEXT(" % d / % d"), SGameState->AlivePlayerControllerCount, SGameState->AlivePlayerControllerCount + DeadPlayerControllers.Num());

			NotifyToAllPlayer(NotificationString);

			if (SGameState->AlivePlayerControllerCount <= 1)
			{
				AlivePlayerControllers[0]->ShowWinnerUI();
				SGameState->MatchState = EMatchState::Ending;
			}
		}
		break;
	}
	case EMatchState::Ending:
	{
		FString NotificationString = FString::Printf(TEXT("Waiting %d for returning to lobby."), RemainWaitingTimeForEnding);

		NotifyToAllPlayer(NotificationString);

		--RemainWaitingTimeForEnding;

		if (0 == RemainWaitingTimeForEnding)
		{
			for (auto AliveController : AlivePlayerControllers)
			{
				AliveController->ReturnToLobby();
			}
			for (auto DeadController : DeadPlayerControllers)
			{
				DeadController->ReturnToLobby();
			}

			MainTimerHandle.Invalidate();	//MainTimer ����

			FName CurrentLevelName = FName(UGameplayStatics::GetCurrentLevelName(this));
			UGameplayStatics::OpenLevel(this, CurrentLevelName, true, FString(TEXT("listen")));
				//���� ���� ������ ���� ���� ���񽺵�� �����Ǿ� �ִٸ� �̷��� ������ �ٽ� �غ�� �� ���Ǽ������� �˷���. "���ο� �÷��̾� ���� �� ����."
				//�׷� ���� ���񽺴� ���ο� �÷��̾�鿡�� ���� ������ IP�ּҸ� �������༭ ���� ����.
		}
		break;
	}
	case EMatchState::End:
		break;
	default:
		break;
	}
}

void ASGameMode::NotifyToAllPlayer(const FString& NotificationString)
{
	for (auto AlivePlayerController : AlivePlayerControllers)
	{
		AlivePlayerController->NotificationText = FText::FromString(NotificationString);
	}

	for (auto DeadPlayerController : DeadPlayerControllers)
	{
		DeadPlayerController->NotificationText = FText::FromString(NotificationString);
	}
}
