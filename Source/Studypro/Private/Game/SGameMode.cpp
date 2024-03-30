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

	if (SGameState->MatchState != EMatchState::Waiting)	//이 방법은 정석이 아님. Waiting일때 접속을 못하게 막음
	{
		NewPlayer->SetLifeSpan(0.1f);	//바로 Destroy하게되면 문제가 발생하여 0.1초 텀을 둠.
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

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);	//1초마다 반복적으로 OnMainTimerElapsed()를 호출.

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

			RemainWaitingTimeForPlaying = WaitingTime;	//최소 인원이 안되면 대기시간 초기화
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

			MainTimerHandle.Invalidate();	//MainTimer 정리

			FName CurrentLevelName = FName(UGameplayStatics::GetCurrentLevelName(this));
			UGameplayStatics::OpenLevel(this, CurrentLevelName, true, FString(TEXT("listen")));
				//만약 데디 서버가 게임 세션 서비스들과 연동되어 있다면 이렇게 레벨을 다시 준비된 뒤 세션서버한테 알려줌. "새로운 플레이어 들어올 수 있음."
				//그럼 세션 서비스는 새로운 플레이어들에게 데디 서버의 IP주소를 전달해줘서 접속 유도.
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
