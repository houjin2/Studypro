// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/SPlayerController.h"

ASPlayerController::ASPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
    //엑터의 Tick이 true여만 입력을 처리 가능
}

void ASPlayerController::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameOnly InputModeGameOnly;   //플레이시 바로 움직일수 있게 함
    SetInputMode(InputModeGameOnly);
}



